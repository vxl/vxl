#include <vector>
#include <iostream>
#include <algorithm>
#include "rgrl_initializer_inv_indexing.h"

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <cassert>

#include <rsdl/rsdl_kd_tree.h>

#include "rgrl_view.h"
#include "rgrl_util.h"
#include "rgrl_invariant.h"
#include "rgrl_invariant_match.h"
#include "rgrl_est_translation.h"

rgrl_initializer_inv_indexing::
rgrl_initializer_inv_indexing( const rgrl_view_sptr& prior_view,
                               bool should_estimate_global_region,
                               int max_num_matches_tried )
  : view_ (prior_view),
    should_estimate_global_region_( should_estimate_global_region ),
    current_moving_image_ind_(0),
    num_matches_tried_( 0 ),
    max_num_matches_tried_( max_num_matches_tried )
{
}

rgrl_initializer_inv_indexing::
rgrl_initializer_inv_indexing( rgrl_mask_sptr const&     from_image_roi,
                               rgrl_mask_sptr const&     to_image_roi,
                               rgrl_estimator_sptr xform_estimator,
                               unsigned             initial_resolution,
                               bool should_estimate_global_region,
                               int max_num_matches_tried )
  : should_estimate_global_region_( should_estimate_global_region ),
    current_moving_image_ind_(0),
    num_matches_tried_( 0 ),
    max_num_matches_tried_( max_num_matches_tried )
{
  // If xform_estimator is not set, assign the simplest estimator possible.
  if ( !xform_estimator ) {
    unsigned dim = from_image_roi->x0().size();
    xform_estimator = new rgrl_est_translation(dim);
  }

  rgrl_mask_box global_region( from_image_roi->bounding_box() );
  view_ = new rgrl_view( from_image_roi, to_image_roi,
                         global_region, global_region,
                         xform_estimator, nullptr,
                         initial_resolution );
}

void
rgrl_initializer_inv_indexing::
add_data( std::vector<rgrl_invariant_sptr> const& fixed_set,
          std::vector<rgrl_invariant_sptr> const& moving_set,
          double nn_radius,
          unsigned int k_nn )
{
  std::vector<std::vector<rgrl_invariant_sptr> > moving_sets;
  moving_sets.push_back( moving_set );
  add_multiple_data( fixed_set, moving_sets, nn_radius , k_nn );
}

void
rgrl_initializer_inv_indexing::
add_multiple_data( std::vector<rgrl_invariant_sptr> const& fixed_set,
                   std::vector<std::vector<rgrl_invariant_sptr> > const& moving_sets,
                   double nn_radius,
                   unsigned int k_nn )
{
  unsigned int num_moving_sets = moving_sets.size();
  assert (fixed_set.size() > 0);

  // allocate space for matches
  if ( matches_.empty() ) matches_.resize( num_moving_sets );
  else assert ( matches_.size() == num_moving_sets );

  // Obtain the number of cartesian, nc, and number of angular, na,
  // from the first invariant feature
  unsigned int nc = fixed_set[0]->cartesian_invariants().size();
  unsigned int na = fixed_set[0]->angular_invariants().size();

  // Construct a kd-tree for the set of search points from the fixed_set
  std::vector<rsdl_point> search_pts(fixed_set.size());
  for (unsigned int pt=0; pt<fixed_set.size(); ++pt) {
    // Set number of expected cartesian and angular values
    search_pts[pt].resize( nc, na );
    search_pts[pt].set_cartesian(fixed_set[pt]->cartesian_invariants());
    search_pts[pt].set_angular(fixed_set[pt]->angular_invariants());
  }
  rsdl_kd_tree kd_tree( search_pts );

  // Iterate through the features from each moving set
  for (unsigned int m_ind = 0; m_ind<num_moving_sets; ++m_ind)
  {
    // Iterate through all constellations/points in the current "from" image
    for (unsigned int pt = 0; pt<moving_sets[m_ind].size(); pt++)
    {
      // Create a query point from the invariants of the current constellation
      rsdl_point query_pt( nc, na );
      query_pt.set_cartesian(moving_sets[m_ind][pt]->cartesian_invariants());
      query_pt.set_angular(moving_sets[m_ind][pt]->angular_invariants());

      // Find all points (and their indices) within nn_radius of the query_pt
      std::vector<rsdl_point> near_neighbor_pts;
      std::vector<int> near_neighbor_indices;
      if ( nn_radius > 0.0 ) {
        kd_tree.points_in_radius( query_pt, nn_radius, near_neighbor_pts,
                                  near_neighbor_indices );
      }
      // If no points were found in nn_radius, find the nearest point
      if (near_neighbor_indices.empty() ) {
        kd_tree.n_nearest( query_pt, k_nn, near_neighbor_pts, near_neighbor_indices );
      }

      // Create matches from the nearest neighbors and push them onto
      // the current vector
      for (int near_neighbor_indice : near_neighbor_indices) {
        matches_[m_ind].push_back( new rgrl_invariant_match(moving_sets[m_ind][pt], fixed_set[near_neighbor_indice]) );
      }
    }
    std::sort(matches_[m_ind].begin(), matches_[m_ind].end(), dist_greater);
  }
}

void
rgrl_initializer_inv_indexing::
set_current_moving_image( unsigned int moving_image_index)
{
  current_moving_image_ind_ = moving_image_index;
  num_matches_tried_ = 0;
}

const std::vector<rgrl_invariant_match_sptr>&
rgrl_initializer_inv_indexing::
matches_for_moving_image( unsigned int moving_image_index)
{
  return matches_[moving_image_index];
}

bool
rgrl_initializer_inv_indexing::
next_initial( rgrl_view_sptr           & view,
              rgrl_scale_sptr          & prior_scale)
{
  if ( max_num_matches_tried_ > 0 &&
       num_matches_tried_ == max_num_matches_tried_ )
    return false;

  rgrl_invariant_match_sptr best_match;
  if (matches_[current_moving_image_ind_].empty())
    return false;

  bool found_best_match;
  // Remove the best (last) match from the vector
  do {
    best_match = matches_[current_moving_image_ind_].back();
    matches_[current_moving_image_ind_].pop_back();
    ++num_matches_tried_;
    found_best_match = best_match->estimate();
  } while ( !found_best_match && !matches_[current_moving_image_ind_].empty());

  if ( !found_best_match ) return false;

  // Determine the global region
  //
  rgrl_mask_box global_region ( view_->from_image_roi()->bounding_box() );
  if ( should_estimate_global_region_ )
    global_region =
      rgrl_util_estimate_global_region(view_->from_image_roi(),
                                       view_->to_image_roi(),
                                       view_->from_image_roi()->bounding_box(),
                                       *best_match->transform());
  // Determine the initial region
  //
  rgrl_mask_box initial_region( view_->from_image_roi()->bounding_box() );
  if ( best_match->has_initial_region() )
    initial_region = best_match->initial_region();

  view = new rgrl_view( view_->from_image_roi(),
                        view_->to_image_roi(),
                        initial_region,
                        global_region,
                        view_->xform_estimator(),
                        best_match->transform(),
                        view_->resolution() );
  prior_scale = rgrl_initializer::enforce_prior_scale( best_match->scale() );

  return true;
}

bool
rgrl_initializer_inv_indexing::
next_initial( rgrl_invariant_match_sptr& best_match )
{
  if ( max_num_matches_tried_ > 0 &&
       num_matches_tried_ == max_num_matches_tried_ )
    return false;

  if (matches_[current_moving_image_ind_].empty())
    return false;

  bool found_best_match;
  // Remove the best (last) match from the vector
  do {
    best_match = matches_[current_moving_image_ind_].back();
    matches_[current_moving_image_ind_].pop_back();
    ++num_matches_tried_;
    found_best_match = best_match->estimate();
  }
  while ( !found_best_match && !matches_[current_moving_image_ind_].empty());

  return found_best_match;
}

int
rgrl_initializer_inv_indexing::
size() const
{
  return int(matches_[current_moving_image_ind_].size());
}
