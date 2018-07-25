// This is brl/bseg/bsgm/bsgm_error_checking.h
#ifndef bsgm_error_checking_h_
#define bsgm_error_checking_h_

//#include <string>
//#include <vector>
//#include <set>
//#include <iostream>
//#include <sstream>
//#include <utility>

#include <vil/vil_image_view.h>

//:
// \file
// \brief Functions for detecting and correcting errors in stereo disparity
//        maps.
// \author Thomas Pollard
// \date July 24, 2018


//: Use the OpenCV SGM uniqueness criteria to find bad disparities. This
// is not quite the same as the left-right consistency check from the SGM
// paper.  Despite working well enough, this approach is problematic in part
// because the overflow-normalized disparity costs of different pixels
// should not be directly compared, but they are.
void bsgm_check_nonunique(
  vil_image_view<float>& disp_img,
  const vil_image_view<unsigned short>& disp_cost,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity,
  vxl_byte shadow_thresh,
  int disp_thresh = 1 );

//: Given two disparity maps, perform the full left-right check from the
// SGM PAMI paper.  Doesn't do occlusion detection yet.
void bsgm_check_leftright(
  const vil_image_view<float>& disp1,
  const vil_image_view<float>& disp2,
  const vil_image_view<bool>& invalid1,
  const vil_image_view<bool>& invalid2,
  vil_image_view<bool>& error1);

//: Fill in disparity pixels flagged as errors via multi-directional
// sampling.
void bsgm_interpolate_errors(
  vil_image_view<float>& disp_img,
  const vil_image_view<bool>& invalid,
  const vil_image_view<vxl_byte>& img,
  float invalid_disparity,
  vxl_byte shadow_thresh);

//: Compute a map of invalid pixels based on seeing the 'border_val'
// in either target or reference images.
void bsgm_compute_invalid_map(
  const vil_image_view<vxl_byte>& img_target,
  const vil_image_view<vxl_byte>& img_ref,
  vil_image_view<bool>& invalid_target,
  int min_disparity,
  int num_disparities,
  vxl_byte border_val = (unsigned char)0 );


//: Flip the sign of all disparities, swap invalid values.
void bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid );


#endif // bsgm_error_checking_h_
