// This is brl/bseg/vpro/vpro_motion_params.cxx
#include <vpro/vpro_motion_params.h>
//:
// \file
// See vpro_motion_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

vpro_motion_params::
vpro_motion_params(const vpro_motion_params& vmp)
  : gevd_param_mixin()
{
  InitParams(vmp.low_range_, vmp.high_range_, vmp.smooth_sigma_);
}

vpro_motion_params::
vpro_motion_params(const float low_range, const float high_range,
                   const float smooth_sigma)
{
  InitParams(low_range, high_range, smooth_sigma);
}

void vpro_motion_params::InitParams(float low_range, float high_range,
                                    float smooth_sigma)
{
  low_range_= low_range;
  high_range_ = high_range;
  smooth_sigma_ = smooth_sigma;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool vpro_motion_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const vpro_motion_params& vmp)
{
  return
  os << "vpro_motion_params:\n[---\n"
     << "low_range " << vmp.low_range_ << vcl_endl
     << "high_range " << vmp.high_range_ << vcl_endl
     << "smooth_sigma " << vmp.smooth_sigma_ << vcl_endl
     << "---]" << vcl_endl;
}
