// This is brl/bseg/sdet/sdet_harris_detector_params.cxx
#include <sdet/sdet_harris_detector_params.h>
//:
// \file
// See sdet_harris_detector_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

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
             hdp.scale_factor_
            );
}

sdet_harris_detector_params::
sdet_harris_detector_params(const float sigma,
                            const float thresh,
                            const int n,
                            const float percent_corners,
                            const float scale_factor
                           )
{
  InitParams(sigma, thresh, n, percent_corners, scale_factor);
}

void sdet_harris_detector_params::InitParams(float sigma,
                                             float thresh,
                                             int n,
                                             float percent_corners,
                                             float scale_factor
                                            )
{
  sigma_= sigma;
  thresh_ = thresh;
  n_ = n;
  percent_corners_ = percent_corners;
  scale_factor_=scale_factor;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_harris_detector_params::SanityCheck()
{
  vcl_stringstream msg;
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
  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<< (vcl_ostream& os, const sdet_harris_detector_params& hdp)
{
  return
  os << "sdet_harris_detector_params:\n[---\n"
     << "sigma " << hdp.sigma_ << vcl_endl
     << "thresh " << hdp.thresh_ << vcl_endl
     << "n " << hdp.n_ << vcl_endl
     << "max_no_corners(percent) " << hdp.percent_corners_ << vcl_endl
     << "scale_factor " << hdp.scale_factor_ << vcl_endl
     << "---]" << vcl_endl;
}
