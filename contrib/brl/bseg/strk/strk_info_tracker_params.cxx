// This is brl/bseg/strk/strk_info_tracker_params.cxx
#include <strk/strk_info_tracker_params.h>
//:
// \file
// See strk_info_tracker_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

strk_info_tracker_params::
strk_info_tracker_params(const strk_info_tracker_params& tp)
  : gevd_param_mixin()
{
  InitParams(tp.n_samples_,
             tp.search_radius_,
             tp.angle_range_,
             tp.scale_range_,
             tp.sigma_,
             tp.gradient_info_,
             tp.color_info_,
             tp.min_gradient_,
             tp.parzen_sigma_,
             tp.use_background_,
             tp.renyi_joint_entropy_,
             tp.verbose_,
             tp.debug_
             );
}

strk_info_tracker_params::
strk_info_tracker_params(const int n_samples,
                         const float search_radius,
                         const float angle_range,
                         const float scale_range,
                         const float sigma,
                         const bool gradient_info,
                         const bool color_info,
                         const float min_gradient,
                         const float parzen_sigma,
                         const bool use_background,
                         const bool renyi_joint_entropy,
                         const bool verbose,
                         const bool debug)
{
  InitParams(n_samples,
             search_radius,
             angle_range,
             scale_range,
             sigma,
             gradient_info,
             color_info,
             min_gradient,
             parzen_sigma,
             use_background,
             renyi_joint_entropy,
             verbose,
             debug);
}

void strk_info_tracker_params::InitParams(int n_samples,
                                          float search_radius,
                                          float angle_range,
                                          float scale_range,
                                          float sigma,
                                          bool gradient_info,
                                          bool color_info,
                                          float min_gradient,
                                          float parzen_sigma,
                                          bool use_background,
                                          bool renyi_joint_entropy,
                                          bool verbose,
                                          bool debug)
{
  n_samples_ = n_samples;
  search_radius_ = search_radius;
  angle_range_ = angle_range;
  scale_range_ = scale_range;
  sigma_ = sigma;
  gradient_info_ = gradient_info;
  color_info_ = color_info;
  min_gradient_ = min_gradient;
  parzen_sigma_ = parzen_sigma;
  use_background_ =   use_background;
  renyi_joint_entropy_ = renyi_joint_entropy;
  verbose_ = verbose;
  debug_ = debug;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool strk_info_tracker_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const strk_info_tracker_params& tp)
{
  os << "strk_info_tracker_params:" << vcl_endl << "[---" << vcl_endl;
  os << "n_samples " << tp.n_samples_ << vcl_endl;
  os << "search_radius " << tp.search_radius_ << vcl_endl;
  os << "angle_range " << tp.angle_range_ << vcl_endl;
  os << "scale_range " << tp.scale_range_ << vcl_endl;
  os << "sigma " << tp.sigma_ << vcl_endl;
  os << "gradient_info " << tp.gradient_info_ << vcl_endl;
  os << "color_info " << tp.color_info_ << vcl_endl;
  os << "min_gradient " << tp.min_gradient_ << vcl_endl;
  os << "parzen_sigma " << tp.parzen_sigma_ << vcl_endl;
  os << "use background model " << tp.use_background_ << vcl_endl;
  os << "renyi joint entropy " << tp.renyi_joint_entropy_ << vcl_endl;
  os << "verbose " << tp.verbose_ << vcl_endl;
  os << "debug " << tp.debug_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
