// This is brl/bseg/strk/strk_info_model_tracker_params.h
#ifndef strk_info_model_tracker_params_h_
#define strk_info_model_tracker_params_h_
//:
// \file
// \brief parameter mixin for strk_info_model_tracker
//
// \author
//    Joseph L. Mundy - November 05, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class strk_info_model_tracker_params : public gevd_param_mixin
{
 public:
  strk_info_model_tracker_params(const int n_samples = 10, 
                                 const float stem_trans_radius = 5.0,
                                 const float long_arm_tip_trans_radius = 5.0,
                                 const float short_arm_tip_trans_radius = 5.0,
                                 const float stem_angle_range = 0.01,
                                 const float long_arm_angle_range = 0.01,
                                 const float short_arm_angle_range = 0.01,
                                 const float long_arm_tip_angle_range = 0.01,
                                 const float short_arm_tip_angle_range = 0.01,
                                 const float sigma = 1.0,
                                 const bool gradient_info = false,
                                 const bool verbose = false
                                 );

  strk_info_model_tracker_params(const strk_info_model_tracker_params& old_params);
  ~strk_info_model_tracker_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream& os, const strk_info_model_tracker_params& tp);
 protected:
  void InitParams(int n_samples, 
                  float stem_trans_radius,
                  float long_arm_tip_trans_radius,
                  float short_arm_tip_trans_radius,
                  float stem_angle_range,
                  float long_arm_angle_range,
                  float short_arm_angle_range,
                  float long_arm_tip_angle_range,
                  float short_arm_tip_angle_range,
                  float sigma,
                  bool gradient_info,
                  bool verbose);
 public:
  //
  // Parameter blocks and parameters
  //
  int n_samples_;          // number of samples generated per iteration
  float stem_trans_radius_;//translation range for stem
  float long_arm_tip_trans_radius_;//translation range for long arm tip
  float short_arm_tip_trans_radius_;//translation range for short arm tip
  float stem_angle_range_;//angle range for stem
  float long_arm_angle_range_;//angle range for long arm shoulder joint
  float short_arm_angle_range_;// angle range for short arm shoulder joint
  float long_arm_tip_angle_range_;// angle range for long arm tip
  float short_arm_tip_angle_range_;// angle range for short arm tip
  float sigma_;// scale range for generating samples
  bool gradient_info_;// Combine gradient and intensity info
  bool verbose_;  // informative messages to cout
};

#endif // strk_info_model_tracker_params_h_
