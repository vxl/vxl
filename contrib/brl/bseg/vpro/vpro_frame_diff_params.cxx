// This is brl/bseg/vpro/vpro_frame_diff_params.cxx
#include <vpro/vpro_frame_diff_params.h>
//:
// \file
// See vpro_frame_diff_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

vpro_frame_diff_params::
vpro_frame_diff_params(const vpro_frame_diff_params& vmp)
  : gevd_param_mixin()
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
  return
  os << "vpro_frame_diff_params:\n[---\n"
     << "thresh " << vmp.thresh_ << vcl_endl
     << "level " << vmp.level_ << vcl_endl
     << "range " << vmp.range_ << vcl_endl
     << "---]" << vcl_endl;
}
