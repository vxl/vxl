// This is bbas/bvgl/algo/bvgl_ptset_ops.h

#ifndef bvgl_ptset_ops_h_
#define bvgl_ptset_ops_h_

//:
// \file
// \brief operations on vgl_pointset_3d
//
// \author J.L. Mundy
// \date Sept. 13, 2019
//
// \verbatim
//  January 29, 2020
//  added noise filtering algorithm
// \endverbatim


#include <iostream>
#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vector>
#include <vgl/vgl_pointset_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
template <class T>
class bvgl_ptset_3d_ops
{

public:
  //: Constructor - default
 bvgl_ptset_3d_ops(){}
 bvgl_ptset_3d_ops(vgl_pointset_3d<T> const& ptset){
   set_ptset(ptset);
 }
 // some algorithms require efficient access to neigboring points
 // this method intializes the k nearest neighbor index that
 // makes use of parallel execution. ptset is assumed to be points only
 void set_ptset(vgl_pointset_3d<T> const& ptset){
    ptset_ = ptset;
    knn_ = bvgl_k_nearest_neighbors_3d<T>(ptset_);
 }
 //: find average distance between neighboring points
 //  used to define a tolerance on distance
 //  for speed only a random fraction of the points is
 //  used to compute average distance
 T average_distance(T frac_ptset = T(1));

 //: roughness value is added as a scalar property of each point
 // roughness is standard deviation of neighboring points from a fitted plane
 // radius determines the set of points (a subset of neighbors with d<radius)
 void compute_roughness(T radius, size_t n_nbrs);

 //: remove points that are outliers to a local planar fit
 void remove_noise(T radius, T dist_tol, size_t n_nbrs);

 //: covariance matrix, C, of a pointset. A random fraction of the points is used for speed.
 static bool covariance_matrix(vgl_pointset_3d<T> const& ptset, vnl_matrix_fixed<T, 3, 3>& C, T frac_ptset = T(1));

 //: find the rotation to diagonalize the covariance matrix, C, of a pointset
 // if the rotation is improper, the sense of the eigenvector with the smallest eigenvalue is reversed
 static void R_to_diagonalize_covar(vnl_matrix_fixed<T, 3, 3> C, vnl_matrix_fixed<T,3,3>& R, std::vector<T>& lambda);
 
 //: rotate a pointset with a 3x3 matrix 
 static vgl_pointset_3d<T> Rtrans(vgl_pointset_3d<T> const& ptset, vnl_matrix_fixed<T,3,3> const& R);

 //: transform a pointset with a similarity transform. translate_first = true means translate then scale and rotate.
 // translate_first = false means rotate and scale then translate.
 static vgl_pointset_3d<T> Strans(vgl_pointset_3d<T> const& ptset, T scale, vnl_matrix_fixed<T,3,3> const& R,
                                  vnl_vector_fixed<T,3> const& t, bool translate_first = true);

 //: accsesors to pointsets that are produced using the knn index
 // i.e., are members of this class
 const vgl_pointset_3d<T>& scalar_result() const {return ptset_with_scalar_result_;}
 const bvgl_k_nearest_neighbors_3d<T>& knn(){return knn_;}
 const vgl_pointset_3d<T>& result(){return ptset_result_;}


 private:
 vgl_pointset_3d<T> ptset_;        // the input pointset assumed not to have normals or scalars
 vgl_pointset_3d<T> ptset_result_; // the output pointset
 vgl_pointset_3d<T> ptset_with_scalar_result_;//output with a generated scalar e.g. scalar is roughness
 bvgl_k_nearest_neighbors_3d<T> knn_;
};


#endif // bbas/bvgl/algo/bvgl_ptset_ops.h
