// This is brl/bseg/vpro/vpro_spatial_filter_params.cxx
#include <vpro/vpro_spatial_filter_params.h>
//:
// \file
// See vpro_spatial_filter_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

vpro_spatial_filter_params::
vpro_spatial_filter_params(const vpro_spatial_filter_params& vsfp)
  : gevd_param_mixin()
{
  InitParams(vsfp.show_filtered_fft_,
             vsfp.dir_fx_, vsfp.dir_fy_, vsfp.f0_, vsfp.radius_);
}

vpro_spatial_filter_params::
vpro_spatial_filter_params(const bool show_filtered_fft,
                           const float dir_fx, const float dir_fy,
                           const float f0, const float radius)
{
  InitParams(show_filtered_fft, dir_fx, dir_fy, f0, radius);
}

void vpro_spatial_filter_params::InitParams(bool show_filtered_fft,
                                            float dir_fx, float dir_fy,
                                            float f0, float radius)
{
  show_filtered_fft_ = show_filtered_fft;
  dir_fx_= dir_fx;
  dir_fy_= dir_fy;
  f0_ = f0;
  radius_ = radius;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool vpro_spatial_filter_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const vpro_spatial_filter_params& vsfp)
{
  return
  os << "vpro_spatial_filter_params:\n[---\n"
     << "show_filtered_fft " << vsfp.show_filtered_fft_ << vcl_endl
     << "dir_fx " << vsfp.dir_fx_ << vcl_endl
     << "dir_fy " << vsfp.dir_fy_ << vcl_endl
     << "f0 " << vsfp.f0_ << vcl_endl
     << "radius " << vsfp.radius_ << vcl_endl
     << "---]" << vcl_endl;
}
