// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef baml_detect_change_h_
#define baml_detect_change_h_

#include <string>
#include <vector>
#include <set>
#include <utility>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil/vil_image_view.h>


//:
// \file
// \brief 
// \author Thomas Pollard
// \date August 22, 2016



<<<<<<< f81b2d29a709425af01bdaa2582cded1a9a5fee8

=======
>>>>>>> Cleaned up baml library, added additional functionality
//: Compute the gain/offset that minimizes the least square distance between
// target and reference images and then correct the reference image.
bool baml_correct_gain_offset(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
<<<<<<< f81b2d29a709425af01bdaa2582cded1a9a5fee8
  const vil_image_view<bool>& valid,
=======
  const vil_image_view<bool>& valid_ref,
>>>>>>> Cleaned up baml library, added additional functionality
  vil_image_view<vxl_uint_16>& corrected_ref );

//: Detect change using the Birchfield-Tomasi metric
bool baml_detect_change_bt(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
<<<<<<< f81b2d29a709425af01bdaa2582cded1a9a5fee8
  const vil_image_view<bool>& valid,
=======
  const vil_image_view<bool>& valid_ref,
>>>>>>> Cleaned up baml library, added additional functionality
  vil_image_view<float>& tar_lh,
  float bt_std = 20.0f,
  int bt_rad = 1 );

//: Detect change using census metric
// census_tol: pixel differences less than this magnitude are not considered
//   in the census computation.  Increase to prevent errors from sensor noise. 
//   Set to 0 for textbook census implementation.
// census_rad: length of the census kernal will be 2*census_rad+1. Must be 
//    1,2,or 3.
bool baml_detect_change_census(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
<<<<<<< f81b2d29a709425af01bdaa2582cded1a9a5fee8
  const vil_image_view<bool>& valid,
=======
  const vil_image_view<bool>& valid_ref,
>>>>>>> Cleaned up baml library, added additional functionality
  vil_image_view<float>& tar_lh,
  float census_std = 0.2f,
  int census_tol = 8,
  int census_rad = 3 );

//: Detect change using the approximate mutual information method described in
// the original semi-global matching paper
bool baml_detect_change_nonparam(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid,
  vil_image_view<float>& tar_lh,
  int img_bit_depth = 12,
  int hist_bit_depth = 10 );

//: Compute the sigmoid function for every pixel in the image i.e.
// prob(x,y) = 1.0/(1.0+exp(-prior_lh*lh(x,y)))
void baml_sigmoid(
  const vil_image_view<float>& lh,
  vil_image_view<float>& prob,
  float prior_prob = 0.1f );

//: Overlay a detection map on a grayscale image using a red coloring
bool baml_overlay_red(
  const vil_image_view<vxl_byte>& img,
  const vil_image_view<vxl_byte>& map,
  vil_image_view<vxl_byte>& vis_img,
  vxl_byte clear_map_val = 0,
  vxl_byte red_map_val = 255 );


#endif // baml_detect_change_h_
