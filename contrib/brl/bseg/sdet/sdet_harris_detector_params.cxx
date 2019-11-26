// This is brl/bseg/sdet/sdet_harris_detector_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_harris_detector_params.h"
//:
// \file
// See sdet_harris_detector_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

//------------------------------------------------------------------------
// Constructors
//

sdet_harris_detector_params::
sdet_harris_detector_params(const sdet_harris_detector_params& hdp)
  : gevd_param_mixin()
{
  InitParams(hdp.sigma_,
             hdp.thresh_,
             hdp.n_,
             hdp.percent_corners_,
             hdp.scale_factor_,
             hdp.use_vil_harris_
            );
}

sdet_harris_detector_params::
sdet_harris_detector_params(const float sigma,
                            const float thresh,
                            const int n,
                            const float percent_corners,
                            const float scale_factor,
                            const bool use_vil_harris
                           )
{
  InitParams(sigma, thresh, n, percent_corners, scale_factor,use_vil_harris);
}

void sdet_harris_detector_params::InitParams(float sigma,
                                             float thresh,
                                             int n,
                                             float percent_corners,
                                             float scale_factor,
                                             bool use_vil_harris
                                            )
{
  sigma_= sigma;
  thresh_ = thresh;
  n_ = n;
  percent_corners_ = percent_corners;
  scale_factor_=scale_factor;
  use_vil_harris_ = use_vil_harris;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool sdet_harris_detector_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (sigma_<0.5)
  {
    msg << "ERROR: smoothing should be effective, >0.5";
    valid = false;
  }
  if (thresh_<0)
  {
    msg << "ERROR: invalid to have a negative threshold";
    valid = false;
  }
  if (n_<1||n_>5)
  {
    msg << "ERROR: should have a reasonable size for the neighborhood";
    valid = false;
  }
  if (percent_corners_<=0||percent_corners_>100)
  {
    msg << "ERROR: value must be a valid percentage";
    valid = false;
  }
  if (scale_factor_<0.01||scale_factor_>0.5)
  {
    msg << "ERROR: scale factor out of range";
    valid = false;
  }
  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator<< (std::ostream& os, const sdet_harris_detector_params& hdp)
{
  return
  os << "sdet_harris_detector_params:\n[---\n"
     << "sigma " << hdp.sigma_ << std::endl
     << "thresh " << hdp.thresh_ << std::endl
     << "n " << hdp.n_ << std::endl
     << "max_no_corners(percent) " << hdp.percent_corners_ << std::endl
     << "scale_factor " << hdp.scale_factor_ << std::endl
     << "vil_harris?" << hdp.use_vil_harris_ << std::endl
     << "---]" << std::endl;
}
