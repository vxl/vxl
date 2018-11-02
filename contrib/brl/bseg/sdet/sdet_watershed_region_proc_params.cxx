// This is brl/bseg/sdet/sdet_watershed_region_proc_params.cxx
#include <iostream>
#include <sstream>
#include "sdet_watershed_region_proc_params.h"
//:
// \file
// See sdet_watershed_region_proc_params.h
//
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool sdet_watershed_region_proc_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (min_area_<2)
  {
    msg << "ERROR: don't use a label array resolution factor less than 2x\n";
    valid = false;
  }

  valid = valid && wp_.SanityCheck();
  msg << wp_.GetErrorMsg() << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_watershed_region_proc_params& p)
{
  return
  os << "sdet_watershed_region_proc_params:\n[---\n" << p.wp_
     << "\n  ---\ndebug " << p.debug_
     << "\nverbose " << p.verbose_
     << "\nmin region area " << p.min_area_
     << "\nregion merge tol " << p.merge_tol_
     << "\nmerge priority " << p.merge_priority_
     << "\n---]\n";
}
