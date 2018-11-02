// This is brl/bseg/sdet/sdet_image_mesh_params.cxx
#include <sstream>
#include <iostream>
#include "sdet_image_mesh_params.h"
//:
// \file
// See sdet_image_mesh_params.h
//
//-----------------------------------------------------------------------------
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

sdet_image_mesh_params::
sdet_image_mesh_params(const sdet_image_mesh_params& imp)
  : gevd_param_mixin(), vbl_ref_count()
{
  InitParams(imp.smooth_, imp.thresh_,
             imp.min_fit_length_,
             imp.rms_distance_,
             imp.step_half_width_);
}

sdet_image_mesh_params::
sdet_image_mesh_params(float smooth, float thresh,
                       int min_fit_length,
                       double rms_distance,
                       double step_half_width)
{
  InitParams(smooth, thresh, min_fit_length, rms_distance, step_half_width);
}

void sdet_image_mesh_params::InitParams(float smooth, float thresh,
                                        int min_fit_length,
                                        double rms_distance,
                                        double step_half_width)
{
  smooth_ = smooth;
  thresh_ = thresh;
  min_fit_length_ = min_fit_length;
  rms_distance_ = rms_distance;
  step_half_width_ = step_half_width;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_image_mesh_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  std::endl otherwise.
  std::stringstream msg;
  bool valid = true;

  if (smooth_ < 0.5f)
  {
    msg << "ERROR: smoothing has no effect!\n";
    valid = false;
  }
  if (thresh_ < 0.0f)
  {
    msg << "ERROR: threshold cannot be negative\n";
    valid = false;
  }
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
  if (step_half_width_<=0)
  {
    msg << "ERROR: infeasible step transition width\n";
    valid = false;
  }
  msg << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_image_mesh_params& imp)
{
  return
  os << "sdet_image_mesh_params:\n[---\n"
     << "smooth sigma" << imp.smooth_ << std::endl
     << "thresh " << imp.thresh_ << std::endl
     << "min fit length " << imp.min_fit_length_ << std::endl
     << "rms distance tolerance" << imp.rms_distance_ << std::endl
     << "step half width" << imp.step_half_width_ << std::endl
     << "---]\n";
}
