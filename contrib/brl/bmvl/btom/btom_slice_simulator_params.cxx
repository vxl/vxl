// This is brl/bmvl/btom/btom_slice_simulator_params.cxx
#include "btom_slice_simulator_params.h"
//:
// \file
// See btom_slice_simulator_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

btom_slice_simulator_params::
btom_slice_simulator_params(const btom_slice_simulator_params& bss)
{
  InitParams(bss.ncyl_,
             bss.min_xy_sigma_,
             bss.max_xy_sigma_
            );
}

btom_slice_simulator_params::
btom_slice_simulator_params(const int ncyl,
                            const float min_xy_sigma,
                            const float max_xy_sigma
                           )
{
  InitParams(ncyl, min_xy_sigma, max_xy_sigma);
}

void btom_slice_simulator_params::InitParams(int ncyl,
                                             float min_xy_sigma,
                                             float max_xy_sigma
                                            )
{
  ncyl_ = ncyl;
  min_xy_sigma_ = min_xy_sigma;
  max_xy_sigma_ = max_xy_sigma;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool btom_slice_simulator_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (ncyl_<=0)
  {
    msg << "ERROR: must have a finite number of cylinders";
    valid = false;
  }

  if (min_xy_sigma_<=0)
  {
    msg << "ERROR: sigma must be finite";
    valid = false;
  }

  if (max_xy_sigma_>100.0)
  {
    msg << "ERROR: max sigma unreasonably large";
    valid = false;
  }

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<<(vcl_ostream& os, const btom_slice_simulator_params& ssp)
{
  os << "btom_slice_simulator_params:\n[---\n"
     << "n cylinders " << ssp.ncyl_ << '\n'
     << "min xy-sigma " << ssp.min_xy_sigma_ << '\n'
     << "max xy_sigma " << ssp.max_xy_sigma_ << '\n'
     << "---]\n";
  return os;
}
