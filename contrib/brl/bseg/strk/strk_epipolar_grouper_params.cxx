// This is brl/bseg/strk/strk_epipolar_grouper_params.cxx
#include <strk/strk_epipolar_grouper_params.h>
//:
// \file
// See strk_epipolar_grouper_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

strk_epipolar_grouper_params::
strk_epipolar_grouper_params(const strk_epipolar_grouper_params& tp)
  : gevd_param_mixin()
{
  InitParams(tp.eu_,
             tp.ev_,
             tp.elu_,
             tp.elv_min_,
             tp.elv_max_,
             tp.Ns_,
             tp.angle_thresh_,
             tp.dummy4_
             );
}

strk_epipolar_grouper_params::
strk_epipolar_grouper_params(const float eu,
                             const float ev,
                             const int elu,
                             const int elv_min,
                             const int elv_max,
                             const int Ns,
                             const float angle_thresh,
                             const bool dummy4)
{
  InitParams(eu,
             ev,
             elu,
             elv_min,
             elv_max,
             Ns,
             angle_thresh,
             dummy4);
}

void strk_epipolar_grouper_params::InitParams(
                                     float eu,
                                     float ev,
                                     int elu,
                                     int elv_min,
                                     int elv_max,
                                     int Ns,
                                     float angle_thresh,
                                     bool  dummy4)
{
  eu_ = eu;
  ev_ = ev;
  elu_ =  elu;
  elv_min_ = elv_min;
  elv_max_ = elv_max;
  Ns_ = Ns;
  angle_thresh_ = angle_thresh;
  dummy4_ = dummy4;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool strk_epipolar_grouper_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const strk_epipolar_grouper_params& tp)
{
  return
  os << "strk_epipolar_grouper_params:\n[---\n"
     << "epi_u " << tp.eu_ << vcl_endl
     << "epi_v " << tp.ev_ << vcl_endl
     << "epi space col " << tp.elu_ << vcl_endl
     << "min epi space row " << tp.elv_min_ << vcl_endl
     << "max epi space row " << tp.elv_max_ << vcl_endl
     << "Samples in s " << tp.Ns_ << vcl_endl
     << "angle_thresh " << tp.angle_thresh_ << vcl_endl
     << "---]" << vcl_endl;
}
