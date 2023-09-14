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
// utils

//: Invalidate disparity pixels with corresponding image intensity below
// a shadow threshold
template <class T>
void bsgm_check_shadows(
  vil_image_view<float>& disp_img,
  const vil_image_view<T>& img,
  float invalid_disparity,
  unsigned short shadow_thresh,
  const vgl_box_2d<int>& img_window = vgl_box_2d<int>());

bool bsgm_generate_line(bool& init, float xs, float ys,
                        float xe, float ye,
                        float& x, float& y,
                        // internal memory 
                        float& dx, float& dy, float& mag,
                        float& xinc, float& yinc,
                        int& x1, int& y1);
// mask the image step response in shadows
// since it produces similar appearance in target and reference
// images, thus creating a false surface at the shadow edge
// of roofs.
template <class T>
void bsgm_shadow_step_filter(
  const vil_image_view<T>& img,
  const vil_image_view<bool>& invalid,
  vil_image_view<float>& step_prob_img, // result
  const vgl_vector_2d<float>& sun_dir,
  int radius,
  int response_low,
  int shadow_high);

// compute shadow probability adaptively based on the result of the
// shadow step filter. Starting at shadow step points continue along
// a scan in the sun direction until the last shadow step point. The rectified
// image intensity at this pixel is used as a shadow threshold until the end
// of the scan. Shadow scan length and shadow step probablity threshold (ss_thresh)
// are parameters.
template <class T>
void bsgm_shadow_prob(vil_image_view<T> const& rect_img,
                      vil_image_view<bool> const& invalid_map,
                      vgl_vector_2d<float> const& sun_dir,
                      float default_shadow_thresh, 
                      vil_image_view<float> const& shadow_step_prob,
                      vil_image_view<float>& shadow_prob,
                      float scan_length, float ss_thresh);

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
