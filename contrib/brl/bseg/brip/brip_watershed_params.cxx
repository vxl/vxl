//:
// \file
#include <brip/brip_watershed_params.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

brip_watershed_params::brip_watershed_params(const brip_watershed_params& wp)
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
  vcl_stringstream msg;
  bool valid = true;

  if (sigma_ <= 0)  // Standard deviation of the smoothing kernel
  {
    msg << "ERROR: Value of gaussian smoothing sigma is too low <=0" << vcl_ends;
    valid = false;
  }

  if (thresh_ < 0)  // Noise weighting factor
  {
    msg << "ERROR: Value of noise weight must be >=0" << vcl_ends;
    valid = false;
  }

  msg << vcl_ends;
  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<<(vcl_ostream& os, const brip_watershed_params& wp)
{
  return
  os << "brip_watershed_params:\n[---\n"
     << "sigma " << wp.sigma_ << vcl_endl
     << "thresh " << wp.thresh_ << vcl_endl
     << "eight connected? " << wp.eight_connected_ << vcl_endl
     << "verbose " << wp.verbose_ << vcl_endl
     << "---]" << vcl_endl;
}
