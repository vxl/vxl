// This is brl/bseg/baml/baml_utilities.h
#ifndef baml_utilities_h_
#define baml_utilities_h_

#include <string>
#include <vector>
#include <set>
#include <utility>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vil/vil_image_view.h>


//:
// \file
// \brief Miscellaneous functions use in change detection.
// \author Thomas Pollard
// \date November 4, 2016


//: Compute the gain/offset that minimizes the least square distance between
// target and reference images and then correct the reference image.
bool baml_correct_gain_offset(
  const vil_image_view<vxl_uint_16>& img_target,
  const vil_image_view<vxl_uint_16>& img_ref,
  const vil_image_view<bool>& valid,
  vil_image_view<vxl_uint_16>& corrected_ref );

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


#endif // baml_utilities_h_
