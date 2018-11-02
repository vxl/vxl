#ifndef boxm2_vecf_landmark_warp_hxx_included_
#define boxm2_vecf_landmark_warp_hxx_included_

#include <iostream>
#include <stdexcept>
#include <vector>
#include <utility>
#include "boxm2_vecf_landmark_warp.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <rsdl/rsdl_kd_tree.h>
#include <rsdl/rsdl_point.h>


std::vector<rsdl_point> convert_vgl_to_rsdl(std::vector<vgl_point_3d<double> > const& pts)
{
  std::vector<rsdl_point> pts_out;
  for (const auto & pt : pts) {
    rsdl_point point(vnl_vector_fixed<double,3>(pt.x(), pt.y(), pt.z()), vnl_vector<double>());
    pts_out.push_back(point);
  }
  return pts_out;
}

template<class F>
boxm2_vecf_landmark_mapper<F>::
boxm2_vecf_landmark_mapper(std::vector<vgl_point_3d<double> > const& control_pts_source,
                           std::vector<vgl_point_3d<double> >  control_pts_target,
                           F weight_function,
                           int n_nearest
                           )
  : control_pts_source_(control_pts_source),
    control_pts_target_(std::move(control_pts_target)),
    weight_function_(weight_function),
    n_nearest_(n_nearest),
    source_kd_tree_(new rsdl_kd_tree(convert_vgl_to_rsdl(control_pts_source), 0, n_nearest))
{
  std::cout << "created kd tree containing " << control_pts_source.size() << " points." << std::endl;
}

template<class F>
boxm2_vecf_landmark_mapper<F>::~boxm2_vecf_landmark_mapper()
{
  if(source_kd_tree_) {
    delete source_kd_tree_;
  }
}

template <class F>
vgl_point_3d<double> boxm2_vecf_landmark_mapper<F>::operator() (vgl_point_3d<double> const& x) const
{
  rsdl_point query_point(vnl_vector_fixed<double,3>(x.x(), x.y(), x.z()), vnl_vector<double>());
  std::vector< rsdl_point > closest_points;
  std::vector< int > indices;
  bool use_heap = true;
  int max_leaves = -1;
  source_kd_tree_->n_nearest( query_point, n_nearest_, closest_points, indices, use_heap, max_leaves );

  vgl_vector_3d<double> vec(0.0, 0.0, 0.0);
  double weight_sum = 0.0;
  for (int n=0; n<n_nearest_; ++n) {
    double dist_sqrd = (control_pts_source_[n] - x).sqr_length();
    double weight = weight_function_(dist_sqrd);
    weight_sum += weight;
    vec += weight*(control_pts_target_[n] - control_pts_source_[n]);
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
boxm2_vecf_landmark_warp<F>::boxm2_vecf_landmark_warp(std::vector<vgl_point_3d<double> >  control_pts_source,
                                                      std::vector<vgl_point_3d<double> >  control_pts_target,
                                                      F weight_function)
: control_pts_source_(std::move(control_pts_source)),
  control_pts_target_(std::move(control_pts_target)),
  weight_function_(weight_function)
{
}

#undef  BOXM2_VECF_LANDMARK_WARP_INSTANTIATE
#define BOXM2_VECF_LANDMARK_WARP_INSTANTIATE(T) \
template class boxm2_vecf_landmark_warp<T >;

#endif
