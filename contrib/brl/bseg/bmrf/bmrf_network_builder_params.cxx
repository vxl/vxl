// This is brl/bseg/bmrf/bmrf_network_builder_params.cxx
#include <bmrf/bmrf_network_builder_params.h>
//:
// \file
// See bmrf_network_builder_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

bmrf_network_builder_params::
bmrf_network_builder_params(const bmrf_network_builder_params& tp)
  : gevd_param_mixin()
{
  InitParams(tp.eu_,
             tp.ev_,
             tp.elu_,
             tp.elv_min_,
             tp.elv_max_,
             tp.Ns_,
             tp.max_delta_recip_s_
             );
}

bmrf_network_builder_params::
bmrf_network_builder_params(const float eu,
                             const float ev,
                             const int elu,
                             const int elv_min,
                             const int elv_max,
                             const int Ns,
                             const float gamma_max)
{
  InitParams(eu,
             ev,
             elu,
             elv_min,
             elv_max,
             Ns,
             gamma_max);
}

void bmrf_network_builder_params::InitParams(
                                     float eu,
                                     float ev,
                                     int elu,
                                     int elv_min,
                                     int elv_max,
                                     int Ns,
                                     float max_delta_recip_s)
{
  eu_ = eu;
  ev_ = ev;
  elu_ =  elu;
  elv_min_ = elv_min;
  elv_max_ = elv_max;
  Ns_ = Ns;
  max_delta_recip_s_ = max_delta_recip_s;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool bmrf_network_builder_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const bmrf_network_builder_params& tp)
{
  return
  os << "bmrf_network_builder_params:\n[---\n"
     << "epi_u " << tp.eu_ << vcl_endl
     << "epi_v " << tp.ev_ << vcl_endl
     << "epi space col " << tp.elu_ << vcl_endl
     << "min epi space row " << tp.elv_min_ << vcl_endl
     << "max epi space row " << tp.elv_max_ << vcl_endl
     << "Samples in s " << tp.Ns_ << vcl_endl
     << "---]" << vcl_endl;
}
