//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/strk/strk_info_model_tracker_params.cxx
#include <strk/strk_info_model_tracker_params.h>
//:
// \file
// See strk_info_model_tracker_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

strk_info_model_tracker_params::
strk_info_model_tracker_params(const strk_info_model_tracker_params& tp)
{
  InitParams(tp.n_samples_,
             tp.stem_trans_radius_,
             tp.long_arm_tip_trans_radius_,
             tp.short_arm_tip_trans_radius_,
             tp.stem_angle_range_,
             tp.long_arm_angle_range_,
             tp.short_arm_angle_range_,
             tp.long_arm_tip_angle_range_,
             tp.short_arm_tip_angle_range_,
             tp.sigma_,
             tp.gradient_info_,
             tp.verbose_);
}

strk_info_model_tracker_params::
strk_info_model_tracker_params(const int n_samples,
                               const float stem_trans_radius,
                               const float long_arm_tip_trans_radius,
                               const float short_arm_tip_trans_radius,
                               const float stem_angle_range,
                               const float long_arm_angle_range,
                               const float short_arm_angle_range,
                               const float long_arm_tip_angle_range,
                               const float short_arm_tip_angle_range,
                               const float sigma,
                               const bool gradient_info,
                               const bool verbose
                               )
{
  InitParams(n_samples,
             stem_trans_radius,
             long_arm_tip_trans_radius,
             short_arm_tip_trans_radius,
             stem_angle_range,
             long_arm_angle_range,
             short_arm_angle_range,
             long_arm_tip_angle_range,
             short_arm_tip_angle_range,
             sigma,
             gradient_info,
             verbose);
}

void 
strk_info_model_tracker_params::InitParams(int n_samples, 
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
                                           bool verbose)
{
  n_samples_ =  n_samples;
  stem_trans_radius_ = stem_trans_radius;
  long_arm_tip_trans_radius_ = long_arm_tip_trans_radius;
  short_arm_tip_trans_radius_ = short_arm_tip_trans_radius;
  stem_angle_range_ = stem_angle_range;
  long_arm_angle_range_ = long_arm_angle_range;
  short_arm_angle_range_ = short_arm_angle_range;
  long_arm_tip_angle_range_ = long_arm_tip_angle_range;
  short_arm_tip_angle_range_ = short_arm_tip_angle_range;
  sigma_ = sigma;
  gradient_info_ = gradient_info;
  verbose_ = verbose;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call use;
//    vcl_endl otherwise.
bool strk_info_model_tracker_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const strk_info_model_tracker_params& tp)
{
  os << "strk_info_model_tracker_params:" << vcl_endl << "[---" << vcl_endl;
  os << "n_samples " << tp.n_samples_ << vcl_endl;
  os << "stem_trans_radius " << tp.stem_trans_radius_ << vcl_endl;
  os << "long_arm_tip_trans_radius " << tp.long_arm_tip_trans_radius_ << vcl_endl;
  os << "short_arm_tip_trans_radius " << tp.short_arm_tip_trans_radius_ << vcl_endl;
  os << "stem_angle_range " << tp.stem_angle_range_ << vcl_endl;
  os << "long_arm_angle_range " << tp.long_arm_angle_range_ << vcl_endl;
  os << "short_arm_angle_range " << tp.short_arm_angle_range_ << vcl_endl;
  os << "long_arm_tip_angle_range " << tp.long_arm_tip_angle_range_ << vcl_endl;
  os << "short_arm_tip_angle_range " << tp.short_arm_tip_angle_range_ << vcl_endl;
  os << "sigma " << tp.sigma_ << vcl_endl;
  os << "gradient_info "<< tp.gradient_info_ << vcl_endl;
  os << "verbose " << tp.verbose_ << vcl_endl;
    os << "---]" << vcl_endl;
  return os;
}
