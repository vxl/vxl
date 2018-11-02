// This is gel/vdgl/vdgl_fit_lines_params.cxx
#include <iostream>
#include "vdgl_fit_lines_params.h"
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool vdgl_fit_lines_params::SanityCheck()
{
  bool valid = true;

  if (min_fit_length_<3)
  {
    std::cerr << "ERROR: need at least 3 points for a fit\n";
    valid = false;
  }
  if (rms_distance_>1)
  {
    std::cerr << "ERROR: a line fit should be better than one pixel rms\n";
    valid = false;
  }

  return valid;
}

std::ostream& operator<<(std::ostream& os, const vdgl_fit_lines_params& flp)
{
  os << "vdgl_fit_lines_params:\n[---\n"
     << "min fit length " << flp.min_fit_length_ << std::endl
     << "rms distance tolerance" << flp.rms_distance_ << std::endl
     << "---]\n";
  return os;
}
