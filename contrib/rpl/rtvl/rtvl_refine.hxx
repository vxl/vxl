// Copyright 2007-2010 Brad King
// Copyright 2007-2008 Chuck Stewart
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file rtvl_license_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef rtvl_refine_hxx
#define rtvl_refine_hxx

#include <iostream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <map>
#include <functional>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "rtvl_refine.h"

#include "rtvl_tensor.h"
#include "rtvl_tokens.h"
#include "rtvl_vote.h"
#include "rtvl_votee.h"
#include "rtvl_voter.h"
#include "rtvl_weight_smooth.h"

#include <rgtl/rgtl_object_array_points.h>
#include <rgtl/rgtl_octree_cell_bounds.h>
#include <rgtl/rgtl_octree_objects.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

//----------------------------------------------------------------------------
template <unsigned int N>
class rtvl_refine_internal
{
 public:
  rtvl_refine_internal(rtvl_refine<N>* e);
  void init(unsigned int num_points, double* points);
  void set_mask_size(double f);
  bool build_next_level();
  void extract_tokens(rtvl_tokens<N>& out) const;
  unsigned int get_vote_count() const { return vote_count; }

 private:
  void setup_level();
  void select_scale();
  void refine_level();
  void select_samples(std::vector<int>& selection);
  bool last_level();

  rtvl_refine<N>* external;
  double scale_multiplier;
  double mask_size;
  std::unique_ptr< rtvl_tokens<N> > level;
  std::unique_ptr< rgtl_octree_objects<N> > objects;
  rgtl_octree_cell_bounds<N> bounds;
  unsigned int vote_count;
};

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_refine_internal<N>::rtvl_refine_internal(rtvl_refine<N>* e): external(e)
{
  scale_multiplier = std::sqrt(2.0);
  mask_size = 1;
  vote_count = 0;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::init(unsigned int num_points, double* points)
{
  // Create the initial level representation.
  level.reset(new rtvl_tokens<N>);

  // Store points in the level.
  level->points.set_number_of_points(num_points);
  for (unsigned int i=0; i < num_points; ++i)
  {
    level->points.set_point(i, points);
    points += N;
  }

  // Initialize all tokens to have ball tensors.
  level->tokens.resize(num_points);

  this->setup_level();
  this->select_scale();
  this->refine_level();
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::set_mask_size(double f)
{
  if (f < 0.01)
  {
    f = 0.01;
  }
  if (f > 1)
  {
    f = 1.0;
  }
  this->mask_size = f;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::setup_level()
{
  // Compute spatial data structure bounds.
  double bds[N][2];
  level->points.compute_bounds(bds);
  bounds.compute_bounds(bds, 1.01);

  // Create a spatial structure for the current point set.
  objects.reset(new rgtl_octree_objects<N>(level->points, bounds, 10));
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::select_scale()
{
  unsigned int const k = 6;
  // Compute a (k+1)-order distance transform so we can find for each
  // input point a kth-closest neighbor.
  objects->compute_distance_transform(k+1);

  // Compute the nearest point to every point.
  unsigned int n = level->points.get_number_of_points();
  std::vector<double> distances(n, 0.0);
  for (unsigned int id=0; id < n; ++id)
  {
    double p[N];
    level->points.get_point(id, p);
    double squared_distances[k+1];
    int nc = objects->query_closest(p, k+1, 0, squared_distances, 0);
    if (nc == (k+1))
    {
      distances[id] = std::sqrt(squared_distances[k]);
    }
  }

  // Choose a scale based on an order-statistic.
  unsigned int nth = static_cast<unsigned int>(distances.size()/10);
  std::nth_element(distances.begin(), distances.begin()+nth, distances.end());
  level->scale = distances[nth];
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::refine_level()
{
  unsigned int n = level->points.get_number_of_points();
  vnl_matrix_fixed<double, N, N> zero(0.0);
  std::vector< vnl_matrix_fixed<double, N, N> > tensors;
  tensors.resize(n, zero);

  rtvl_weight_smooth<N> tvw(level->scale);

  vote_count = 0;
  for (unsigned int i=0; i < n; ++i)
  {
    vnl_vector_fixed<double, N> voter_location;
    level->points.get_point(i, voter_location.data_block());
    rtvl_voter<N> voter(voter_location, level->tokens[i]);

    // Lookup points within reach of the voter.
    std::vector<int> votee_ids;
    int num_votees = objects->query_sphere(voter_location.data_block(),
                                           3*level->scale, votee_ids);

    // Cast a vote at every votee.
    vnl_vector_fixed<double, N> votee_location;
    for (int vi=0; vi < num_votees; ++vi)
    {
      unsigned int j = votee_ids[vi];
      if (j == i) { continue; }
      level->points.get_point(j, votee_location.data_block());
      rtvl_votee<N> votee(votee_location, tensors[j]);
      rtvl_vote(voter, votee, tvw, true);
    }
    vote_count += num_votees;
  }

  // Update the level with the refined tokens.
  for (unsigned int i=0; i < n; ++i)
  {
    level->tokens[i].set_tensor(tensors[i]);
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::select_samples(std::vector<int>& selection)
{
  unsigned int n = level->points.get_number_of_points();

  // Queue the tokens ordered from most to least salient.
  typedef std::multimap<double, unsigned int , std::greater<double> > saliency_map_type;
  saliency_map_type saliency_map;
  std::vector<saliency_map_type::iterator> saliency_map_index;
  saliency_map_index.resize(n, saliency_map.end());
  for (unsigned int i=0; i < n; ++i)
  {
    rtvl_tensor<N> const& tensor = level->tokens[i];
    double s = tensor.lambda(0);
    saliency_map_type::value_type entry(s, i);
    saliency_map_index[i] = saliency_map.insert(entry);
  }

  while (!saliency_map.empty())
  {
    // Select the next-most-salient sample.
    unsigned int id = saliency_map.begin()->second;
    selection.push_back(id);

    // Lookup points covered by the masking sphere.
    double p[N];
    level->points.get_point(id, p);
    std::vector<int> masked_ids;
    int num_masked = objects->query_sphere(p, level->scale*mask_size,
                                           masked_ids);

    // Remove the masked points.
    for (int i=0; i < num_masked; ++i)
    {
      saliency_map_type::iterator& mi = saliency_map_index[masked_ids[i]];
      if (mi != saliency_map.end())
      {
        saliency_map.erase(mi);
        mi = saliency_map.end();
      }
    }
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
bool rtvl_refine_internal<N>::last_level()
{
  // This is the last level if the scale covers the entire bounds.
  return this->bounds.size() <= (6*level->scale);
}

//----------------------------------------------------------------------------
template <unsigned int N>
bool rtvl_refine_internal<N>::build_next_level()
{
  // Stop if this is already the last level.
  if (this->last_level())
  {
    return false;
  }

  // Select samples for inclusion in the next level.
  std::vector<int> selection;
  this->select_samples(selection);
  unsigned int n = static_cast<unsigned int>(selection.size());

  // We are done with spatial queries at this level.
  objects.reset(0);

  // Create the representation for the next level.
  std::unique_ptr< rtvl_tokens<N> > next_level(new rtvl_tokens<N>);

  // Increment the scale for the next level.
  next_level->scale = level->scale * scale_multiplier;

  // Initialize tokens in the next level with ball tensors.
  next_level->tokens.resize(n);

  // Copy selected samples to the next level.
  next_level->points.set_number_of_points(n);
  for (unsigned int i=0; i < n; ++i)
  {
    unsigned int id = selection[i];
    double p[N];
    level->points.get_point(id, p);
    next_level->points.set_point(i, p);

    // Communicate the tensor to the next level.
    double const max_saliency = 10;
    next_level->tokens[i] = level->tokens[id];
    next_level->tokens[i].next_scale(scale_multiplier, max_saliency);
  }

  // We are done with the selection.
  selection.clear();

  // Replace the current level with the next level.
  level = std::move(next_level);
  this->setup_level();
  this->refine_level();

  return true;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine_internal<N>::extract_tokens(rtvl_tokens<N>& out) const
{
  // Identify the salient tokens.
  std::vector<unsigned int> salient;
  unsigned int n = level->points.get_number_of_points();
  for (unsigned int id=0; id < n; ++id)
  {
    // Get the sum of all saliencies.
    double s = level->tokens[id].lambda(0);

    // TODO: What to use as threshold here?
    if (s > 1.0)
    {
      salient.push_back(id);
    }
  }

  // Copy the salient tokens into the output.
  out.scale = level->scale;
  out.points.set_number_of_points(int(salient.size()));
  out.tokens.resize(salient.size());
  for (std::vector<unsigned int>::const_iterator si = salient.begin();
       si != salient.end(); ++si)
  {
    unsigned int in_id = *si;
    unsigned int out_id = static_cast<unsigned int>(si - salient.begin());
    double p[N];
    level->points.get_point(in_id, p);
    out.points.set_point(out_id, p);
    out.tokens[out_id] = level->tokens[in_id];
  }
}

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_refine<N>::rtvl_refine(unsigned int num_points, double* points):
  internal_(0)
{
  std::unique_ptr<internal_type> internal_p(new internal_type(this));
  internal_p->init(num_points, points);
  this->internal_ = internal_p.release();
}

//----------------------------------------------------------------------------
template <unsigned int N>
rtvl_refine<N>::~rtvl_refine()
{
  delete this->internal_;
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine<N>::set_mask_size(double f)
{
  this->internal_->set_mask_size(f);
}

//----------------------------------------------------------------------------
template <unsigned int N>
void rtvl_refine<N>::get_tokens(rtvl_tokens<N>& tokens) const
{
  this->internal_->extract_tokens(tokens);
}

//----------------------------------------------------------------------------
template <unsigned int N>
unsigned int rtvl_refine<N>::get_vote_count() const
{
  return this->internal_->get_vote_count();
}

//----------------------------------------------------------------------------
template <unsigned int N>
bool rtvl_refine<N>::next_scale()
{
  return this->internal_->build_next_level();
}

//----------------------------------------------------------------------------
#define RTVL_REFINE_INSTANTIATE(N) \
  template class rtvl_refine_internal<N>; \
  template class rtvl_refine<N>

#endif
