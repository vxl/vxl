//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/vpro/vpro_fourier_params.cxx
#include <vpro/vpro_fourier_params.h>
//:
// \file
// See vpro_fourier_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

vpro_fourier_params::
vpro_fourier_params(const vpro_fourier_params& vfp)
{
  InitParams(vfp.thresh_, vfp.level_, vfp.range_);
}

vpro_fourier_params::
vpro_fourier_params(const float thresh, const float level, const float range)

{
  InitParams(thresh, level, range);
}

void vpro_fourier_params::InitParams(float thresh, float level, float range)

{
  thresh_= thresh;
  level_ = level;
  range_ = range;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool vpro_fourier_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const vpro_fourier_params& vfp)
{
  os << "vpro_fourier_params:" << vcl_endl << "[---" << vcl_endl;
  os << "thresh " << vfp.thresh_ << vcl_endl;
  os << "level " << vfp.level_ << vcl_endl;
  os << "range " << vfp.range_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
