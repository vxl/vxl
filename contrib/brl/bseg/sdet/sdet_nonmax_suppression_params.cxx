// This is brl/bseg/sdet/sdet_nonmax_suppression_params.cxx
#include "sdet_nonmax_suppression_params.h"
//:
// \file
// See sdet_nonmax_suppression_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_nonmax_suppression_params::
sdet_nonmax_suppression_params(const sdet_nonmax_suppression_params& nsp)
  : gevd_param_mixin()
{
  InitParams(nsp.thresh_, nsp.pfit_type_);
}

sdet_nonmax_suppression_params::
sdet_nonmax_suppression_params(const double thresh, const int pfit_type)
{
  InitParams(thresh, pfit_type);
}

void sdet_nonmax_suppression_params::InitParams(double thresh, int pfit_type)
{
  thresh_ = thresh;
  pfit_type_ = pfit_type;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_nonmax_suppression_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (thresh_<0 || thresh_>100)
  {
    msg << "ERROR: percentage threshold should be between 0 and 100";
    valid = false;
  }
  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<< (vcl_ostream& os, const sdet_nonmax_suppression_params& nsp)
{
  return
  os << "sdet_nonmax_suppression_params:\n[---\n"
     << "Gradient threshold in percentage " << nsp.thresh_ << vcl_endl
     << "---]" << vcl_endl;
}
