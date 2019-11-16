// This is brl/bbas/bvgl/bvgl_k_nearest_neighbors_2d.h
#ifndef bvgl_k_nearest_neighbors_2d_h_
#define bvgl_k_nearest_neighbors_2d_h_
//:
// \file
// \brief Uses the nabo knn algorithm to find nearest neighbors
// \author Daniel Crispell
//

#include <iostream>
#include <iosfwd>
#include <limits>
#include <algorithm>
#include <utility>
#include <memory>
#include <stdexcept>
#include <bnabo/bnabo.h>
#include <vgl/vgl_point_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
class bvgl_k_nearest_neighbors_2d
{
public:
  //: Construct from set of points
  bvgl_k_nearest_neighbors_2d(
      std::vector<vgl_point_2d<Type> > const &ptset,
      Type tolerance = Type(0)
      );

  //: check class validity
  bool is_valid() const;

  //: query for the closest point (k = 1) including self
  bool closest_point(vgl_point_2d<Type> const& p, vgl_point_2d<Type>& cp) const;

  //: query for the index in the pointset with the closest point including self
  bool closest_index(vgl_point_2d<Type> const& p, unsigned& index) const;

  //: find the indices of the k closest neighbors (limited by max_dist)
  bool knn_indices(
      vgl_point_2d<Type> const& p,
      unsigned k,
      std::vector<unsigned> &neighbor_indices,
      Type max_dist = std::numeric_limits<Type>::infinity()
      ) const;

  //: find k nearest neighbors
  bool knn(
      vgl_point_2d<Type> const& p,
      unsigned k,
      std::vector<vgl_point_2d<Type>>& neighbor_locs,
      Type max_dist = std::numeric_limits<Type>::infinity()
      ) const;

  //: find k nearest neighbors and indices
  bool knn(
      vgl_point_2d<Type> const& p,
      unsigned k,
      std::vector<vgl_point_2d<Type>>& neighbor_locs,
      std::vector<unsigned>& neighbor_indices,
      Type max_dist = std::numeric_limits<Type>::infinity()
      ) const;

protected:
  //: creates and populates the underlying data structure
  bool create();

  //: main utility function - discover "k" nearest neighbors
  // to point p within max_dist radius; return locations &indices
  bool knn_util(
      vgl_point_2d<Type> const& p,
      unsigned k,
      std::vector<vgl_point_2d<Type>>& neighbor_locs,
      std::vector<unsigned>& neighbor_indices,
      Type max_dist = std::numeric_limits<Type>::infinity()
      ) const;

  //: "closest" utility function - single nearest neighbor
  bool closest_util(
      vgl_point_2d<Type> const& p,
      vgl_point_2d<Type>& cp,
      unsigned& ci
      ) const;

  // private member variables
  Type tolerance_ = Type(0);
  std::unique_ptr<Nabo::NearestNeighbourSearch<Type>> search_tree_ = nullptr;
  vnl_matrix<Type> M_; //a matrix form (2 x n) of the pointset used by nabo
  std::vector<vgl_point_2d<Type>> ptset_;
  unsigned flags_ = Type(0); //control various actions during queries
};


// constructor
template <class Type>
bvgl_k_nearest_neighbors_2d<Type>::bvgl_k_nearest_neighbors_2d(
    std::vector<vgl_point_2d<Type>> const& ptset,
    Type tolerance):
  tolerance_(tolerance),
  ptset_(ptset)
{
  this->create();
}

// creates and populates the underlying data structure
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::create()
{
  // invalidate underlying data structure
  if (search_tree_) {
    search_tree_ = nullptr;
  }

  // search flags
  flags_ = 0;
  flags_ = flags_ |  Nabo::NearestNeighbourSearch<Type>::ALLOW_SELF_MATCH;

  // check input pointset
  unsigned n = ptset_.size(), dim = 2;
  if (n==0) {
    return false;
  }

  // populate matrix form(2 x n) of the pointset
  M_.set_size(dim, n);
  for(unsigned i = 0; i<n; ++i){
    vgl_point_2d<Type> pi = ptset_[i];
    M_[0][i]=pi.x();
    M_[1][i]=pi.y();
  }

  // create underlying data structure
  search_tree_.reset(Nabo::NearestNeighbourSearch<Type>::createKDTreeLinearHeap(M_, dim));
  return true;
}

// return validity of underlying data structure
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::is_valid() const
{
  if(!search_tree_)
    return false;
  else
    return true;
}

// return single closest index
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::closest_index(
    vgl_point_2d<Type> const& p,
    unsigned& ci
    ) const
{
  vgl_point_2d<Type> cp;
  return this->closest_util(p, cp, ci);
}

// return single closest point
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::closest_point(
    vgl_point_2d<Type> const& p,
    vgl_point_2d<Type>& cp
    ) const
{
  unsigned ci = 0;
  return this->closest_util(p, cp, ci);
}

// return neighbor indices only
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn_indices(
    vgl_point_2d<Type> const& p,
    unsigned k,
    std::vector<unsigned> &neighbor_indices,
    Type max_dist
    ) const
{
  std::vector<vgl_point_2d<Type>> neighbor_locs;
  return this->knn_util(p, k, neighbor_locs, neighbor_indices, max_dist);
}

// return neighbor locations only
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn(
    vgl_point_2d<Type> const& p,
    unsigned k,
    std::vector<vgl_point_2d<Type>>& neighbor_locs,
    Type max_dist
    ) const
{
  std::vector<unsigned> neighbor_indices;
  return this->knn_util(p, k, neighbor_locs, neighbor_indices, max_dist);
}

// return neighbor locations & indices
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn(
    vgl_point_2d<Type> const& p,
    unsigned k,
    std::vector<vgl_point_2d<Type>>& neighbor_locs,
    std::vector<unsigned>& neighbor_indices,
    Type max_dist
    ) const
{
  return this->knn_util(p, k, neighbor_locs, neighbor_indices, max_dist);
}


// "closest" utility function - discover closest neighbor
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::closest_util(
    vgl_point_2d<Type> const& p,
    vgl_point_2d<Type>& cp,
    unsigned& ci
    ) const
{
  std::vector<unsigned> neighbor_indices;
  std::vector<vgl_point_2d<Type>> neighbor_locs;
  if (!this->knn_util(p, 1, neighbor_locs, neighbor_indices))
    return false;

  cp = neighbor_locs[0];
  ci = neighbor_indices[0];
  return true;
}


// main utility function - discover "k" nearest neighbors
// to point p within max_dist radius; return locations &indices
template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn_util(
    vgl_point_2d<Type> const& p,
    unsigned k,
    std::vector<vgl_point_2d<Type>>& neighbor_locs,
    std::vector<unsigned>& neighbor_indices,
    Type max_dist
    ) const
{
  // clear output
  neighbor_indices.clear();
  neighbor_locs.clear();

  // check search tree validity
  if(!this->is_valid()) {
    return false;
  }

  // variable init
  vnl_vector<Type> q(2), dists2(k);
  vnl_vector<int> indices(k);
  q[0]=p.x();
  q[1]=p.y();

  // search
  search_tree_->knn(q, indices, dists2, k, tolerance_, flags_, max_dist);

  // gather output
  for (unsigned i = 0; i < k; ++i) {

    // infinite distance or invalid index == no more neighbors found
    // if max_dist is finite, fewer than k neighbors is fine
    // if max_dist is infinite, fewer than k neighbors is unexpected
    if(!std::isfinite(dists2[i]) || indices[i]<0) {
      if (std::isfinite(max_dist)) {
        break;
      } else {
        return false;
      }
    }

    // add valid index to list
    auto index = static_cast<unsigned>(indices[i]);
    neighbor_locs.push_back(ptset_[index]);
    neighbor_indices.push_back(index);
  }

  // success
  return true;
}


#endif // bvgl_k_nearest_neighbors_2d_h_
