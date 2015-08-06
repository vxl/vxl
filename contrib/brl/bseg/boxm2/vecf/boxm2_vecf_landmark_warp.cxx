#include "boxm2_vecf_landmark_warp.h"
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boct/boct_bit_tree.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_stdexcept.h>
#include <vcl_utility.h> // for make_pair


// instantiate the template code in the .txx file
#include "boxm2_vecf_vector_field.txx"
BOXM2_VECF_VECTOR_FIELD_INSTANTIATE(boxm2_vecf_landmark_mapper);

boxm2_vecf_landmark_mapper::
boxm2_vecf_landmark_mapper(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                           vcl_vector<vgl_point_3d<double> > const& control_pts_target)
  : control_pts_source_(control_pts_source),
    control_pts_target_(control_pts_target)
{}

vgl_point_3d<double> boxm2_vecf_landmark_mapper::operator() (vgl_point_3d<double> const& x) const
{
  // compute weighted sum of control points
  const double epsilon = 1e-1;
  vgl_vector_3d<double> vec(0.0, 0.0, 0.0);
  double weight_sum = 0.0;
  for (vcl_vector<vgl_point_3d<double> >::const_iterator 
       spit=control_pts_source_.begin(), tpit = control_pts_target_.begin();
       spit != control_pts_source_.end(); ++spit, ++tpit) {
    double dist_sqrd = (*spit - x).sqr_length();
    double weight = 1.0 / (dist_sqrd + epsilon);
    weight_sum += weight;
    vec += weight*(*tpit - *spit);
  }
  vec /= weight_sum;
  return x + vec;
}


//: Create a function object that maps source pts to target pts.
boxm2_vecf_landmark_mapper
boxm2_vecf_landmark_warp::make_forward_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id)
{
  return boxm2_vecf_landmark_mapper(control_pts_source_, control_pts_target_);
}

//: Create a function object that maps target pts to source pts.
boxm2_vecf_landmark_mapper 
boxm2_vecf_landmark_warp::make_inverse_mapper(boxm2_scene_sptr source, boxm2_block_id const& blk_id)
{
  // swap source and target points to get inverse (ish) transformation
  return boxm2_vecf_landmark_mapper(control_pts_target_, control_pts_source_);
}

boxm2_vecf_landmark_warp::boxm2_vecf_landmark_warp(vcl_vector<vgl_point_3d<double> > const& control_pts_source,
                                                   vcl_vector<vgl_point_3d<double> > const& control_pts_target)
: control_pts_source_(control_pts_source),
  control_pts_target_(control_pts_target)
{
}

