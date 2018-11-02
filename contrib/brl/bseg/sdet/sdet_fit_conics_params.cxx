// This is brl/bseg/sdet/sdet_fit_conics_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_fit_conics_params.h"
//:
// \file
// See sdet_fit_conics_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

sdet_fit_conics_params::
sdet_fit_conics_params(const sdet_fit_conics_params& flp)
  : gevd_param_mixin(), vbl_ref_count()
{
  InitParams(flp.min_fit_length_,
             flp.rms_distance_,flp.aspect_ratio_);
}

sdet_fit_conics_params::
sdet_fit_conics_params(int min_fit_length,
                      double rms_distance,int aspect_ratio)
{
  InitParams(min_fit_length, rms_distance,aspect_ratio);
}

void sdet_fit_conics_params::InitParams(int min_fit_length,
                                       double rms_distance,double aspect_ratio)
{
  min_fit_length_ = min_fit_length;
  rms_distance_ = rms_distance;
  aspect_ratio_ = aspect_ratio;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_fit_conics_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  std::endl otherwise.
  std::stringstream msg;
  bool valid = true;

  if (min_fit_length_<3)
  {
    msg << "ERROR: need at least 3 points for a fit\n";
    valid = false;
  }
  if (rms_distance_>1)
  {
    msg << "ERROR: a line fit should be better than one pixel rms\n";
    valid = false;
  }
  if (aspect_ratio_>30)
  {
    msg << "ERROR: better to have an aspect ratio cutoff less than 10\n";
    valid = false;
  }

  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_fit_conics_params& flp)
{
  return
  os << "sdet_fit_conics_params:\n[---\n"
     << "min fit length " << flp.min_fit_length_ << std::endl
     << "rms distance tolerance" << flp.rms_distance_ << std::endl
     <<"aspect ratio tolerance" <<flp.aspect_ratio_ <<std::endl
     << "---]\n";
}
