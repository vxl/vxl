// This is brl/bseg/sdet/sdet_fit_lines_params.cxx
#include <sdet/sdet_fit_lines_params.h>
//:
// \file
// See sdet_fit_lines_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_fit_lines_params::
sdet_fit_lines_params(const sdet_fit_lines_params& flp)
  : gevd_param_mixin(), vbl_ref_count()
{
  InitParams(flp.min_fit_length_,
             flp.rms_distance_);
}

sdet_fit_lines_params::
sdet_fit_lines_params(int min_fit_length,
                      double rms_distance)
{
  InitParams(min_fit_length, rms_distance);
}

void sdet_fit_lines_params::InitParams(int min_fit_length,
                                       double rms_distance)
{
  min_fit_length_ = min_fit_length;
  rms_distance_ = rms_distance;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_fit_lines_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  vcl_endl otherwise.
  vcl_stringstream msg;
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
   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_fit_lines_params& flp)
{
  os << "sdet_fit_lines_params:\n[---\n"
     << "min fit length " << flp.min_fit_length_ << vcl_endl
     << "rms distance tolerance" << flp.rms_distance_ << vcl_endl
     << "---]\n";
  return os;
}
