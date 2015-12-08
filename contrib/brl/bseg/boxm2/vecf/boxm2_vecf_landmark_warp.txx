#ifndef boxm2_vecf_landmark_warp_txx_included_
#define boxm2_vecf_landmark_warp_txx_included_

#include "boxm2_vecf_landmark_warp.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_stdexcept.h>
#include <vcl_utility.h> // for make_pair

template<class F>
boxm2_vecf_landmark_mapper<F>::
boxm2_vecf_landmark_mapper(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                           vcl_vector<vgl_point_3d<double> > const& control_pts_target,
                           F weight_function
                           )
  : control_pts_source_(control_pts_source),
    control_pts_target_(control_pts_target),
    weight_function_(weight_function)
{}

template <class F>
vgl_point_3d<double> boxm2_vecf_landmark_mapper<F>::operator() (vgl_point_3d<double> const& x) const
{
  vgl_vector_3d<double> vec(0.0, 0.0, 0.0);
  double weight_sum = 0.0;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator 
       spit=control_pts_source_.begin(), tpit = control_pts_target_.begin();
       spit != control_pts_source_.end(); ++spit, ++tpit) {
    double dist_sqrd = (*spit - x).sqr_length();
    double weight = weight_function_(dist_sqrd);
    weight_sum += weight;
    vec += weight*(*tpit - *spit);
  }
  vec /= weight_sum;
  return x + vec;
}

//: Create a function object that maps source pts to target pts.
template <class F>
boxm2_vecf_landmark_mapper<F>
boxm2_vecf_landmark_warp<F>::make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id)
{
  return boxm2_vecf_landmark_mapper<F>(control_pts_source_, control_pts_target_, weight_function_);
}

//: Create a function object that maps target pts to source pts.
template <class F>
boxm2_vecf_landmark_mapper<F>
boxm2_vecf_landmark_warp<F>::make_inverse_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id)
{
  // swap source and target points to get inverse (ish) transformation
  return boxm2_vecf_landmark_mapper<F>(control_pts_target_, control_pts_source_, weight_function_);
}

template <class F>
boxm2_vecf_landmark_warp<F>::boxm2_vecf_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                                                      vcl_vector<vgl_point_3d<double> > const& control_pts_target,
                                                      F weight_function)
: control_pts_source_(control_pts_source),
  control_pts_target_(control_pts_target),
  weight_function_(weight_function)
{
}

#undef  BOXM2_VECF_LANDMARK_WARP_INSTANTIATE
#define BOXM2_VECF_LANDMARK_WARP_INSTANTIATE(T) \
template class boxm2_vecf_landmark_warp<T >;

#endif
