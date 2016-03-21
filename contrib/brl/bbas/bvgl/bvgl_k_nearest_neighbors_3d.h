// This is brl/bbas/bvgl/bvgl_k_nearest_neighbors_3d.h
#ifndef bvgl_k_nearest_neighbors_3d_h_
#define bvgl_k_nearest_neighbors_3d_h_
//:
// \file
// \brief Uses the nabo knn algorithm to find nearest neighbors
// \author February 22, 2016 J.L. Mundy
//

#include <vgl/vgl_pointset_3d.h>
#include <bnabo/bnabo.h>
#include <vcl_iosfwd.h>
#include <vcl_limits.h>
template <class Type>
class bvgl_k_nearest_neighbors_3d
{
 public:
  //: default constructor
 bvgl_k_nearest_neighbors_3d():tolerance_(Type(0)), search_tree_(0){}
  //: Construct from a vgl_pointset
  bvgl_k_nearest_neighbors_3d(vgl_pointset_3d<Type> const& ptset, Type tolerance = Type(0));
  //: destructor
  ~bvgl_k_nearest_neighbors_3d(){
    if(search_tree_)
      delete search_tree_;
    search_tree_ = 0;
  }
  //: create search tree
  // use this method if adding a pointset to an existing default k_nearest_neighbors instance
  bool create();

  //: query for the closest point (k = 1) including self
  bool closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& cp) const;

  //: query for the index in the pointset with the closest point including self
  // useful if the source point set has normals that are to retrived for the closest point
  bool closest_index(vgl_point_3d<Type> const& p, unsigned& index) const;

  //: find k nearest neighbors. if the source pointset has normals they are included in the returned pointset
  bool knn(vgl_point_3d<Type> const& p, unsigned k, vgl_pointset_3d<Type>& neighbors) const;

  //: accessors provide efficient access to a single pointset copy
  const vgl_pointset_3d<Type>& const_ptset() const {return ptset_;}
  vgl_pointset_3d<Type>& ptset(){return ptset_;}
  void set_pointset(vgl_pointset_3d<Type> const & ptset){
    ptset_ = ptset;
    this->create();
  }
  protected:
  Type tolerance_;
  Nabo::NearestNeighbourSearch<Type>* search_tree_;
  vnl_matrix<Type> M_;//a matrix form(3 x n) of the pointset used by nabo
  vgl_pointset_3d<Type> ptset_;
  unsigned flags_;//control various actions during queries
};
template <class Type>
bool bvgl_k_nearest_neighbors_3d<Type>::create(){
  if(search_tree_){
    delete search_tree_;
    search_tree_ = 0;
  }
  flags_ = 0;
  flags_ = flags_ |  Nabo::NearestNeighbourSearch<Type>::ALLOW_SELF_MATCH;
  unsigned n = ptset_.npts(), dim = 3;
  if(n==0){
    search_tree_ = 0;
    return false;
  }
  M_.set_size(dim, n);
  for(unsigned i = 0; i<n; ++i){
    vgl_point_3d<Type> pi = ptset_.p(i);
    M_[0][i]=pi.x();    M_[1][i]=pi.y();    M_[2][i]=pi.z();
  }
  search_tree_ = Nabo::NearestNeighbourSearch<Type>::createKDTreeLinearHeap(M_, dim);
  return true;
}
template <class Type>
bvgl_k_nearest_neighbors_3d<Type>::bvgl_k_nearest_neighbors_3d(vgl_pointset_3d<Type> const& ptset, Type tolerance = Type(0)):
search_tree_(0), tolerance_(tolerance), ptset_(ptset){
  create();
}
template <class Type>
bool bvgl_k_nearest_neighbors_3d<Type>::closest_index(vgl_point_3d<Type> const& p, unsigned& index) const{
  unsigned k = 1;
  vnl_vector<int> indices(k);
  vnl_vector<Type> q(3),dists2(k);
  q[0]=p.x();  q[1]=p.y();  q[2]=p.z();
  if(!search_tree_)
    return false;
  search_tree_->knn(q, indices,dists2, k, tolerance_, flags_);
  if(dists2[0] == vcl_numeric_limits<Type>::infinity()||indices[0]<0)
    return false;
  index = static_cast<unsigned>(indices[0]);
  return true;
}
template <class Type>
bool bvgl_k_nearest_neighbors_3d<Type>::closest_point(vgl_point_3d<Type> const& p, vgl_point_3d<Type>& cp) const{
  unsigned index=0;
  if(!this->closest_index(p, index))
    return false;
  cp = ptset_.p(index);
  return true;
}

template <class Type>
bool bvgl_k_nearest_neighbors_3d<Type>::knn(vgl_point_3d<Type> const& p, unsigned k, vgl_pointset_3d<Type>& neighbors) const{
  vnl_vector<int> indices(k);
  vnl_vector<Type> q(3),dists2(k);
  q[0]=p.x();  q[1]=p.y();  q[2]=p.z();
  if(!search_tree_)
    return false;
  search_tree_->knn(q, indices,dists2, k, tolerance_, flags_);
  bool has_normals = ptset_.has_normals();
  for(unsigned i = 0; i<k; ++i){
    if(dists2[i] == vcl_numeric_limits<Type>::infinity()||indices[i]<0)
      return false;
    unsigned indx = static_cast<unsigned>(indices[i]);
    if(has_normals)
      neighbors.add_point_with_normal(ptset_.p(indx), ptset_.n(indx));
    else
      neighbors.add_point(ptset_.p(indx));
  }
  return true;
}
#endif // bvgl_k_nearest_neighbors_3d_h_
