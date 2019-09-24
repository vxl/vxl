// This is bbas/bvgl/algo/bvgl_ptset_ops.h

#ifndef bvgl_ptset_ops_h_
#define bvgl_ptset_ops_h_

//:
// \file
// \brief operations on vgl_pointset_x
//
// \author J.L. Mundy
// \date Sept. 13, 2019
//
// \verbatim
//  Modifications:None
// \endverbatim


#include <iostream>
#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_pointset_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>

template <class T>
class bvgl_ptset_3d_ops
{

public:
  //: Constructor - default
 bvgl_ptset_3d_ops(){}
 bvgl_ptset_3d_ops(vgl_pointset_3d<T> const& ptset){
   set_ptset(ptset);
 }
 void set_ptset(vgl_pointset_3d<T> const& ptset){
    ptset_ = ptset;
    knn_ = bvgl_k_nearest_neighbors_3d<T>(ptset_);
 }
 void compute_roughness(T radius, size_t n_nbrs);
 const vgl_pointset_3d<T>& scalar_result() const {return ptset_with_scalar_result_;}
 const bvgl_k_nearest_neighbors_3d<T>& knn(){return knn_;}
 private:
  size_t n_nbrs_;
  vgl_pointset_3d<T> ptset_;
  vgl_pointset_3d<T> ptset_with_scalar_result_;
  bvgl_k_nearest_neighbors_3d<T> knn_;
};


#endif // bbas/bvgl/algo/bvgl_ptset_ops.h
