// This is brl/bseg/sdet/sdet_vehicle_finder_params.cxx
#include <sdet/sdet_vehicle_finder_params.h>
//:
// \file
// See sdet_vehicle_finder_params.h
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
bool sdet_vehicle_finder_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (search_radius_<5)
  {
    msg << "ERROR: unrealistic to use a search radius< 5\n";
    valid = false;
  }

  valid = valid && wrpp_.SanityCheck();
  msg << wrpp_.GetErrorMsg() << vcl_ends;
  
  valid = valid && pcp_.SanityCheck();
  msg << pcp_.GetErrorMsg() << vcl_ends;
  
  SetErrorMsg(msg.str().c_str());
  return valid;
}

 vcl_ostream& operator << (vcl_ostream& os, sdet_vehicle_finder_params const& p)
{
  return
  os << "sdet_vehicle_finder_params:\n[---\n" << p.wrpp_
     << '\n' << p.pcp_
     << "\n  ---\ndebug " << p.debug_
     << "\nverbose " << p.verbose_
     << "\nsearch radius " << p.search_radius_
     << "\nshadow thresh " << p.shadow_thresh_
     << "\ndistance scale " << p.distance_scale_
     << "\npara cvrg thresh " << p.para_thresh_
     << "\n---]\n";
}
