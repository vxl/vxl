#include <sstream>
#include <iostream>
#include "brip_watershed_params.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

brip_watershed_params::brip_watershed_params(const brip_watershed_params& wp)
: gevd_param_mixin(wp)
{
  InitParams(wp.sigma_, wp.thresh_,
             wp.eight_connected_, wp.verbose_);
}

brip_watershed_params::brip_watershed_params(float sigma, float thresh,
                                             bool eight_connected,
                                             bool verbose)
{
  InitParams(sigma, thresh, eight_connected, verbose);
}

void brip_watershed_params::InitParams(float sigma, float thresh,
                                       bool eight_connected, bool verbose)
{
  sigma_ = sigma;
  thresh_ = thresh;
  eight_connected_ = eight_connected;
  verbose_ = verbose;
}

//-----------------------------------------------------------------------------
//
//: Checks that parameters are within acceptable bounds
//
bool brip_watershed_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (sigma_ <= 0)  // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << std::ends;
    valid = false;
  }

  if (thresh_ < 0)  // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be >=0" << std::ends;
    valid = false;
  }

  msg << std::ends;
  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator<<(std::ostream& os, const brip_watershed_params& wp)
{
  return
  os << "brip_watershed_params:\n[---\n"
     << "sigma " << wp.sigma_ << std::endl
     << "thresh " << wp.thresh_ << std::endl
     << "eight connected? " << wp.eight_connected_ << std::endl
     << "verbose " << wp.verbose_ << std::endl
     << "---]" << std::endl;
}
