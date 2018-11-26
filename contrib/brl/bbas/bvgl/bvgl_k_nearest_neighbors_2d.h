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
  bvgl_k_nearest_neighbors_2d(std::vector<vgl_point_2d<Type> > const &ptset, Type tolerance = Type(0));

  //: query for the closest point (k = 1) including self
  bool closest_point(vgl_point_2d<Type> const& p, vgl_point_2d<Type>& cp) const;

  //: query for the index in the pointset with the closest point including self
  bool closest_index(vgl_point_2d<Type> const& p, unsigned& index) const;

  //: find k nearest neighbors.
  bool knn(vgl_point_2d<Type> const& p, unsigned k, std::vector<vgl_point_2d<Type>>& neighbors) const;

  //: find k nearest neighbors and indices
  bool knn(vgl_point_2d<Type> const& p, unsigned k, std::vector<vgl_point_2d<Type>>& neighbors, vnl_vector<int> &indices) const;

  //: find the indices of the k closest neighbors.
  bool knn_indices(vgl_point_2d<Type> const& p, unsigned k, vnl_vector<int> &indices) const;

protected:
  //: creates and populates the underlying data structure
  bool create();

  Type tolerance_;
  std::unique_ptr<Nabo::NearestNeighbourSearch<Type>> search_tree_;
  vnl_matrix<Type> M_;//a matrix form(2 x n) of the pointset used by nabo
  std::vector<vgl_point_2d<Type>> ptset_;
  unsigned flags_;//control various actions during queries
};


template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::create(){
  flags_ = 0;
  flags_ = flags_ |  Nabo::NearestNeighbourSearch<Type>::ALLOW_SELF_MATCH;
  unsigned n = ptset_.size(), dim = 2;
  if(n==0){
    search_tree_ = nullptr;
    return false;
  }
  M_.set_size(dim, n);
  for(unsigned i = 0; i<n; ++i){
    vgl_point_2d<Type> pi = ptset_[i];
    M_[0][i]=pi.x();
    M_[1][i]=pi.y();
  }
  search_tree_.reset(Nabo::NearestNeighbourSearch<Type>::createKDTreeLinearHeap(M_, dim));
  return true;
}


template <class Type>
bvgl_k_nearest_neighbors_2d<Type>::bvgl_k_nearest_neighbors_2d(std::vector<vgl_point_2d<Type>> const& ptset, Type tolerance):
tolerance_(tolerance),
ptset_(ptset)
{
  create();
}


template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn_indices(vgl_point_2d<Type> const& p, unsigned k, vnl_vector<int> &indices) const
{
  indices.set_size(k);
  vnl_vector<Type> q(2), dists2(k);
  q[0]=p.x();
  q[1]=p.y();
  if(!search_tree_) {
    return false;
  }
  search_tree_->knn(q, indices, dists2, k, tolerance_, flags_);
  if(dists2[k-1] == std::numeric_limits<Type>::infinity()||indices[k-1]<0) {
    return false;
  }
  return true;
}


template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::closest_index(vgl_point_2d<Type> const& p, unsigned& index) const{
  unsigned k = 1;
  vnl_vector<int> indices(k);
  vnl_vector<Type> q(2), dists2(k);
  q[0]=p.x();
  q[1]=p.y();
  if(!search_tree_)
    return false;
  search_tree_->knn(q, indices,dists2, k, tolerance_, flags_);
  if(dists2[0] == std::numeric_limits<Type>::infinity()||indices[0]<0)
    return false;
  index = static_cast<unsigned>(indices[0]);
  return true;
}


template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::closest_point(vgl_point_2d<Type> const& p, vgl_point_2d<Type>& cp) const{
  unsigned index=0;
  if(!this->closest_index(p, index))
    return false;
  cp = ptset_[index];
  return true;
}


template <class Type>
bool bvgl_k_nearest_neighbors_2d<Type>::knn(vgl_point_2d<Type> const& p, unsigned k, std::vector<vgl_point_2d<Type>>& neighbors) const{
  vnl_vector<int> indices(k);
  return knn(p, k, neighbors, indices);
}


template <class Type>
inline bool bvgl_k_nearest_neighbors_2d<Type>::knn(vgl_point_2d<Type> const& p, unsigned k, std::vector<vgl_point_2d<Type>>& neighbors, vnl_vector<int> &indices) const {
  neighbors.clear();
  vnl_vector<Type> q(2), dists2(k);
  q[0]=p.x();
  q[1]=p.y();
  if(!search_tree_) {
    return false;
  }
  search_tree_->knn(q, indices, dists2, k, tolerance_, flags_);
  for(unsigned i = 0; i<k; ++i){
    if(dists2[i] == std::numeric_limits<Type>::infinity()||indices[i]<0) {
      return false;
    }
    unsigned indx = static_cast<unsigned>(indices[i]);
    neighbors.push_back(ptset_[indx]);
  }
  return true;
}

#endif // bvgl_k_nearest_neighbors_2d_h_
