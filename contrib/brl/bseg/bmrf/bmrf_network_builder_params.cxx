//----*-c++-*----tells emacs to use C++ mode----------
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
{
  InitParams(tp.eu_,
             tp.ev_,
             tp.elu_,
             tp.elv_min_,
             tp.elv_max_,
             tp.Ns_,
             tp.alpha_range_,
             tp.gamma_max_
             );
}

bmrf_network_builder_params::
bmrf_network_builder_params(const float eu,
                             const float ev,
                             const int elu,
                             const int elv_min,
                             const int elv_max,
                             const int Ns,
                             const float alpha_range,
                             const float gamma_max)
{
  InitParams(eu,
             ev,
             elu,
             elv_min,
             elv_max,
             Ns,
             alpha_range,
             gamma_max);
}

void bmrf_network_builder_params::InitParams(
                                     float eu,
                                     float ev,
                                     int elu,
                                     int elv_min,
                                     int elv_max,
                                     int Ns,
                                     float alpha_range,
                                     float  gamma_max)

{
  eu_ = eu;
  ev_ = ev;
  elu_ =  elu;
  elv_min_ = elv_min;
  elv_max_ = elv_max;
  Ns_ = Ns;
  alpha_range_ = alpha_range;
  gamma_max_ = gamma_max;
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
  os << "bmrf_network_builder_params:" << vcl_endl << "[---" << vcl_endl;
  os << "epi_u " << tp.eu_ << vcl_endl;
  os << "epi_v " << tp.ev_ << vcl_endl;
  os << "epi space col " << tp.elu_ << vcl_endl;
  os << "min epi space row " << tp.elv_min_ << vcl_endl;
  os << "max epi space row " << tp.elv_max_ << vcl_endl;
  os << "Samples in s " << tp.Ns_ << vcl_endl;
  os << "alpha_range " << tp.alpha_range_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
