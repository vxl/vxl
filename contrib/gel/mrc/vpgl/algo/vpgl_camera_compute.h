// This is gel/mrc/vpgl/algo/vpgl_camera_compute.h
#ifndef _vpgl_camera_compute_h_
#define _vpgl_camera_compute_h_
//:
// \file
// \brief Several routines for computing different kinds of cameras from world-point
// correspondences.
// \author Thomas Pollard
// \date 7/18/05
//
// Should template this class.

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>

//: Basic least squares solution for a general projective camera given corresponding 
// world and image points.
class vpgl_proj_camera_compute
{
public:

  vpgl_proj_camera_compute(){};

  //: Compute from two sets of corresponding points, put the resulting camera
  // into camera, return true if successful.
  bool compute( const vcl_vector< vgl_homg_point_2d<double> >& image_pts,
                const vcl_vector< vgl_homg_point_3d<double> >& world_pts,
                vpgl_proj_camera<double>& camera );
  bool compute( const vcl_vector< vgl_point_2d<double> >& image_pts,
                const vcl_vector< vgl_point_3d<double> >& world_pts,
                vpgl_proj_camera<double>& camera );

};


//: Basic least squares solution for an affine camera given corresponding 
// world and image points.
class vpgl_affine_camera_compute
{
public:

  vpgl_affine_camera_compute(){};

  //: Compute from two sets of corresponding points, put the resulting camera
  // into camera, return true if successful.
  bool compute( const vcl_vector< vgl_point_2d<double> >& image_pts,
                const vcl_vector< vgl_point_3d<double> >& world_pts,
                vpgl_affine_camera<double>& camera );

};


#endif //_vpgl_camera_compute_h_
