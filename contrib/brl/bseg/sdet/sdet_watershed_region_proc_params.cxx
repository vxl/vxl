// This is brl/bseg/sdet/sdet_watershed_region_proc_params.cxx
#include <sdet/sdet_watershed_region_proc_params.h>
//:
// \file
// See sdet_watershed_region_proc_params.h
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_sstream.h>

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_watershed_region_proc_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (min_area_<2)
  {
    msg << "ERROR: don't use a label array resolution factor less than 2x\n";
    valid = false;
  }

  valid = valid && wp_.SanityCheck();
  msg << wp_.GetErrorMsg() << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_watershed_region_proc_params& p)
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
