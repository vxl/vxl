// This is brl/bseg/bsgm/bsgm_error_checking.h
#ifndef bsgm_error_checking_h_
#define bsgm_error_checking_h_

//#include <string>
//#include <set>
//#include <iostream>
//#include <sstream>
//#include <utility>

#include <vgl/vgl_box_2d.h>
#include <vil/vil_image_view.h>
#include <vector>

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
template <class T>
void bsgm_check_nonunique(
  vil_image_view<float>& disp_img,
  const vil_image_view<unsigned short>& disp_cost,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  int disp_thresh = 1,
  const vgl_box_2d<int>& img_window = vgl_box_2d<int>());


//: Given two disparity maps, perform the full left-right check from the
// SGM PAMI paper.  Doesn't do occlusion detection yet.
void bsgm_check_leftright(
  const vil_image_view<float>& disp1,
  const vil_image_view<float>& disp2,
  const vil_image_view<bool>& invalid1,
  const vil_image_view<bool>& invalid2,
  vil_image_view<bool>& error1);


//: Compute a map of invalid pixels based on seeing the 'border_val'
// in either target or reference images.
template <class T >
void bsgm_compute_invalid_map(
  const vil_image_view<T>& img_tar,
  const vil_image_view<T>& img_ref,
  vil_image_view<bool>& invalid_tar,
  int min_disparity,
  int num_disparities,
  T border_val = T(0),
  const vgl_box_2d<int>& target_window = vgl_box_2d<int>());

//: Fill in disparity pixels flagged as errors via multi-directional
// sampling.
template <class T>
void bsgm_interpolate_errors(
  vil_image_view<float>& disp_img,
  const vil_image_view<bool>& invalid,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  const vgl_box_2d<int>& img_window = vgl_box_2d<int>());


//: Flip the sign of all disparities, swap invalid values.
void bsgm_invert_disparities(
  vil_image_view<float>& disp_img,
  int old_invalid,
  int new_invalid );

#define BSGM_ERROR_CHECKING_INSTANTIATE(T) extern "please include bsgm/bsgm_error_checking.hxx first"
#endif // bsgm_error_checking_h_
