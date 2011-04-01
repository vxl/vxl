// This is brl/bseg/sdet/sdet_denoise_mrf_bp_params.cxx
#include "sdet_denoise_mrf_bp_params.h"
//:
// \file
// See sdet_denoise_mrf_bp_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_denoise_mrf_bp_params::
sdet_denoise_mrf_bp_params(const sdet_denoise_mrf_bp_params& dmp)
  : gevd_param_mixin(), vbl_ref_count()
{
  InitParams(dmp.n_labels_, dmp.n_iter_, dmp.pyramid_levels_, dmp.discontinuity_cost_,
             dmp.truncation_cost_, dmp.kappa_, dmp.lambda_);

}

sdet_denoise_mrf_bp_params::
sdet_denoise_mrf_bp_params(unsigned n_labels,
                           unsigned n_iter, unsigned pyramid_levels,
                           float discontinuity_cost,
                           float truncation_cost,
                           float kappa,
                           float lambda )
{
  InitParams(n_labels, n_iter, pyramid_levels, discontinuity_cost,
             truncation_cost, kappa, lambda);
}

void sdet_denoise_mrf_bp_params::
InitParams(unsigned n_labels, unsigned n_iter, unsigned pyramid_levels,
           float discontinuity_cost, float truncation_cost,
           float kappa, float lambda)
{
  n_labels_ = n_labels;
  n_iter_ = n_iter;
  pyramid_levels_ = pyramid_levels;
  discontinuity_cost_ = discontinuity_cost;
  truncation_cost_ = truncation_cost;
  kappa_ = kappa; 
  lambda_ = lambda;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
bool sdet_denoise_mrf_bp_params::SanityCheck()
{
  //  Note that msg << ends seems to restart the string and erase the
  //  previous string. We should only use it as the last call, use
  //  vcl_endl otherwise.
  vcl_stringstream msg;
  bool valid = true;

  if (n_labels_ < 2)
    {
      msg << "ERROR: at least 2 labels are needed!\n";
      valid = false;
    }

  if (n_iter_ < 1)
    {
      msg << "ERROR: at least 1 iteration is needed!\n";
      valid = false;
    }
  if ( pyramid_levels_ < 1)
    {
      msg << "ERROR: need at least one pyramid level\n";
      valid = false;
    }
  if (discontinuity_cost_ < 0.0f)
    {
      msg << "ERROR: discontinuity cost can't be negative\n";
      valid = false;
    }
  if (truncation_cost_ < 0.0f)
    {
      msg << "ERROR: discontinuity cost can't be negative\n";
      valid = false;
    }
  if (kappa_ < 0.0f)
    {
      msg << "ERROR: kappa can't be negative\n";
      valid = false;
    }

  if (lambda_ < 0.0f)
    {
      msg << "ERROR: lambda can't be negative\n";
      valid = false;
    }

  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_denoise_mrf_bp_params& dmp)
{
  return
  os << "sdet_denoise_mrf_bp_params:\n[---\n"
     << "n labels " << dmp.n_labels_ << vcl_endl
     << "n iterations " << dmp.n_iter_ << vcl_endl
     << "pyramid levels " << dmp.pyramid_levels_ << vcl_endl
     << "discontinuity cost " << dmp.discontinuity_cost_ << vcl_endl
     << "truncation cost " << dmp.truncation_cost_ << vcl_endl
     << "kappa  " << dmp.kappa_ << vcl_endl
     << "lambda  " << dmp.lambda_ << vcl_endl
     << "---]\n";
}
