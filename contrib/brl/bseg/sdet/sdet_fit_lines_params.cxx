//----*-c++-*----tells emacs to use C++ mode----------
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
{
  InitParams(flp.min_fit_length_,
             flp.rms_distance_);
}

sdet_fit_lines_params::
sdet_fit_lines_params(const double min_fit_length,
                      const double rms_distance)
{
  InitParams(min_fit_length, rms_distance);
}

void sdet_fit_lines_params::InitParams(double min_fit_length,
                                       double rms_distance)
                                       
{
  min_fit_length_ = min_fit_length;
  rms_distance_ = rms_distance;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_fit_lines_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if(min_fit_length_<3)
    {
      msg << "ERROR: need at least 3 points for a fit";
      valid = false;
    }
  if(rms_distance_>1)
    {
      msg << "ERROR: a line fit should be better than one pixel rms";
      valid = false;
    }
   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_fit_lines_params& flp)
{
  os << "sdet_fit_lines_params:" << vcl_endl << "[---" << vcl_endl;
  os << "min fit length " << flp.min_fit_length_ << vcl_endl;
  os << "rms distance tolerance" << flp.rms_distance_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
