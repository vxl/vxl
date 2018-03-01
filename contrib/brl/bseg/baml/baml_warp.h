// This is brl/bseg/baml/baml_warp.h
#ifndef baml_warp_h_
#define baml_warp_h_

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vpgl/vpgl_camera.h>

//:
// \file
// \brief Various warping functions.
// \author Thomas Pollard
// \date July 1, 2016


//: A basic perspective warp, for convenience
void baml_warp_perspective(
  const vil_image_view<vxl_uint_16>& img1,
  const vgl_h_matrix_2d<double>& img1_to_img2,
  int img2_width,
  int img2_height,
  vil_image_view<vxl_uint_16>& img2,
  bool inverse_homography = false );

void baml_warp_perspective(
  const vil_image_view<float>& img1,
  const vgl_h_matrix_2d<double>& img1_to_img2,
  int img2_width,
  int img2_height,
  vil_image_view<float>& img2,
  bool inverse_homography = false );
//: Warp image 1 into image 2 assuming a planar mapping.
bool baml_warp_via_ground_plane(
  const vil_image_view<vxl_uint_16>& img1,
  const vpgl_camera<double>& cam1,
  const vgl_box_2d<int>& img2_region,
  const vpgl_camera<double>& cam2,
  float z_world,
  vil_image_view<vxl_uint_16>& img2 );

//: Warp image 1 into image 2 via a digital elevation map.
bool baml_warp_via_dem(
  const vil_image_view<vxl_uint_16>& img1,
  const vpgl_camera<double>& cam1,
  int img2_width,
  int img2_height,
  const vpgl_camera<double>& cam2,
  const vgl_box_2d<double>& bounding_box_world,
  const vil_image_view<float>& dem,
  const vgl_h_matrix_2d<double>& dem_to_world,
  vil_image_view<vxl_uint_16>& img2,
  vil_image_view<bool>& valid2 );

#endif // baml_warp_h_
