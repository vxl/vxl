// This is brl/bseg/sdet/sdet_denoise_mrf_params.cxx
#include "sdet_denoise_mrf_params.h"
//:
// \file
// See sdet_denoise_mrf_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_denoise_mrf_params::
sdet_denoise_mrf_params(const sdet_denoise_mrf_params& dmp)
  : gevd_param_mixin(), vbl_ref_count()
{
  InitParams(dmp.radius_, dmp.kappa_, dmp.beta_);

}

sdet_denoise_mrf_params::
sdet_denoise_mrf_params(double radius, double kappa, double beta)
{
  InitParams(radius, kappa, beta);
}

void sdet_denoise_mrf_params::InitParams(double radius,
                                         double kappa,
                                         double beta)
{
  radius_ = radius;
  kappa_ = kappa;
  beta_ = beta;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_denoise_mrf_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  vcl_endl otherwise.
  vcl_stringstream msg;
  bool valid = true;

  if (radius_ < 1.0)
    {
      msg << "ERROR: no binary cliques!\n";
      valid = false;
    }
  if (kappa_ < 0.0f)
    {
      msg << "ERROR: kappa can't be negative\n";
      valid = false;
    }
  if (beta_ < 0.0f)
    {
      msg << "ERROR: beta can't be negative\n";
      valid = false;
    }
  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_denoise_mrf_params& dmp)
{
  return
  os << "sdet_denoise_mrf_params:\n[---\n"
     << "radius" << dmp.radius_ << vcl_endl
     << "kappa " << dmp.kappa_ << vcl_endl
     << "beta " << dmp.beta_ << vcl_endl
     << "---]\n";
}
