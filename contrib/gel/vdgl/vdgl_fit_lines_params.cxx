// This is gel/vdgl/vdgl_fit_lines_params.cxx
#include <vdgl/vdgl_fit_lines_params.h>
//:
// \file

#include <vcl_iostream.h>

bool vdgl_fit_lines_params::SanityCheck()
{
  bool valid = true;

  if (min_fit_length_<3)
    {
      vcl_cerr << "ERROR: need at least 3 points for a fit\n";
      valid = false;
    }
  if (rms_distance_>1)
    {
      vcl_cerr << "ERROR: a line fit should be better than one pixel rms\n";
      valid = false;
    }

  return valid;
}

vcl_ostream& operator<<(vcl_ostream& os, const vdgl_fit_lines_params& flp)
{
  os << "vdgl_fit_lines_params:\n[---\n"
     << "min fit length " << flp.min_fit_length_ << vcl_endl
     << "rms distance tolerance" << flp.rms_distance_ << vcl_endl
     << "---]\n";
  return os;
}
