// This is brl/bseg/bsgm/bsgm_disparity_estimator.h
#ifndef bsgm_multiscale_disparity_estimator_h_
#define bsgm_multiscale_disparity_estimator_h_

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <sstream>
#include <utility>

#include <vul/vul_timer.h>
#include <vnl/vnl_math.h>
#include <vil/vil_image_view.h>

#include "bsgm_disparity_estimator.h"

//:
// \file
// \brief A multi-scale implementation of SGM.
// \author Thomas Pollard
// \date June 7, 2016
//
//  Modifications
//   Jun, 2016 Yi Dong - add input parameter for function 'compute'
// \endverbatim


class bsgm_multiscale_disparity_estimator
{
 public:

  //: Construct from parameters. Coarse scale SGM will run on images
  // downsampled by 2^downscale_exponent
  bsgm_multiscale_disparity_estimator(
    const bsgm_disparity_estimator_params& params,
    int img_width,
    int img_height,
    int num_disparities,
    int num_active_disparities,
    int downscale_exponent = 2 );

  //: Destructor
  ~bsgm_multiscale_disparity_estimator();

  //: Run SGM twice, once at a lower resolution to determine the active
  // disparity range, and again at full-res using the reduced disparity range.
  // Should improve speed and quality with respect to single-scale approach if
  // tight disparity bounds are unknown.
  // mutli_scale_mode can be:
  // 0 Single min disparity used for entire image
  // 1 Single min disparity used within coarse image blocks
  // 2 Different min disparity used at each pixel
  bool compute(
    const vil_image_view<vxl_byte>& img_target,
    const vil_image_view<vxl_byte>& img_ref,
    const vil_image_view<bool>& invalid_target,
    int min_disparity,
    float invalid_disparity,
    int const& multi_scale_mode,
    vil_image_view<float>& disp_target);

 protected:

  //: Size of image
  int coarse_w_, coarse_h_, fine_w_, fine_h_;

  //: Downscaling parameters where downscale_factor_ = 2^downscale_exponent_
  int downscale_exponent_;
  int downscale_factor_;

  int num_coarse_disparities_, num_fine_disparities_, num_active_disparities_;

  //: Single-scale SGMs for coarse and fine scales
  bsgm_disparity_estimator* coarse_de_;
  bsgm_disparity_estimator* fine_de_;
};



#endif // bsgm_multiscale_disparity_estimator_h_
