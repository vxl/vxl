//----*-c++-*----tells emacs to use C++ mode----------
// This is brl/bseg/vpro/vpro_frame_diff_params.cxx
#include <vpro/vpro_frame_diff_params.h>
//:
// \file
// See vpro_frame_diff_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

vpro_frame_diff_params::
vpro_frame_diff_params(const vpro_frame_diff_params& vmp)
{
  InitParams(vmp.thresh_, vmp.level_, vmp.range_);
}

vpro_frame_diff_params::
vpro_frame_diff_params(const float thresh, const float level, const float range)

{
  InitParams(thresh, level, range);
}

void vpro_frame_diff_params::InitParams(float thresh, float level, float range)

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
bool vpro_frame_diff_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const vpro_frame_diff_params& vmp)
{
  os << "vpro_frame_diff_params:" << vcl_endl << "[---" << vcl_endl;
  os << "thresh " << vmp.thresh_ << vcl_endl;
  os << "level " << vmp.level_ << vcl_endl;
  os << "range " << vmp.range_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
