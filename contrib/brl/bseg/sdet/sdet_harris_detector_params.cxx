//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/sdet/sdet_harris_detector_params.cxx
#include <sdet/sdet_harris_detector_params.h>
//:
// \file
// See sdet_harris_detector_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_harris_detector_params::
sdet_harris_detector_params(const sdet_harris_detector_params& hdp)
{
  InitParams(hdp.sigma_,
             hdp.thresh_,
             hdp.n_,
             hdp.n_corners_,
             hdp.scale_factor_
             );
}

sdet_harris_detector_params::
sdet_harris_detector_params(const float sigma, 
                            const float thresh,
                            const int n,
                            const int n_corners,
                            const float scale_factor
                            )
{
  InitParams(sigma, thresh, n, n_corners, scale_factor);
}

void sdet_harris_detector_params::InitParams(float sigma,
                                             float thresh,
                                             int n,
                                             int n_corners,
                                             float scale_factor
                                             )
{
  sigma_= sigma;
  thresh_ = thresh;
  n_ = n;
  n_corners_ = n_corners;
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

  if(sigma_<0.5)
    {
      msg << "ERROR: smoothing should be effective, >0.5";
      valid = false;
    }
  if(thresh_<0)
    {
      msg << "ERROR: invalid to have a negative threshold";
      valid = false;
    }
  if(n_<1||n_>5)
    {
      msg << "ERROR: should have a reasonable size for the neighborhood";
      valid = false;
    }
  if(n_corners_<=0)
    {
      msg << "ERROR: should have at least 1 corner";
      valid = false;
    }

  if(scale_factor_<0.01||scale_factor_>0.5)
    {
      msg << "ERROR: scale factor out of range";
      valid = false;
    }
    
   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_harris_detector_params& hdp)
{
  os << "sdet_harris_detector_params:" << vcl_endl << "[---" << vcl_endl;
  os << "sigma " << hdp.sigma_ << vcl_endl;
  os << "thresh " << hdp.thresh_ << vcl_endl;
  os << "n " << hdp.n_ << vcl_endl;
  os << "max_no_corners " << hdp.n_corners_ << vcl_endl;
  os << "scale_factor " << hdp.scale_factor_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
