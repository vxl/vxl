// This is brl/bseg/sdet/sdet_vehicle_finder_params.cxx
#include <sdet/sdet_vehicle_finder_params.h>
//:
// \file
// See sdet_vehicle_finder_params.h
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_vehicle_finder_params::
sdet_vehicle_finder_params(const sdet_vehicle_finder_params& vfp)
{
  InitParams((sdet_watershed_region_proc_params)vfp.wrpp_,
             (brip_para_cvrg_params)vfp.pcp_,
             vfp.verbose_,
             vfp.debug_,
             vfp.search_radius_,
             vfp.shadow_thresh_,
             vfp.para_thresh_,
             vfp.distance_scale_
            );
}

sdet_vehicle_finder_params::
sdet_vehicle_finder_params(const sdet_watershed_region_proc_params& wrpp,
                           const brip_para_cvrg_params& pcp,
                           bool verbose,
                           bool debug,
                           float search_radius,
                           float shadow_thresh,
                           float para_thresh,
                           float distance_scale
                           )
{
  InitParams(wrpp, pcp, verbose, debug, search_radius, shadow_thresh,
             para_thresh, distance_scale);
}

void sdet_vehicle_finder_params::
InitParams(const sdet_watershed_region_proc_params& wrpp,
           const brip_para_cvrg_params& pcp,
           bool verbose,
           bool debug,
           float search_radius,
           float shadow_thresh,
           float para_thresh,
           float distance_scale)
{
  search_radius_ = search_radius;
  shadow_thresh_ = shadow_thresh;
  para_thresh_ = para_thresh;
  distance_scale_ = distance_scale;
  debug_ = debug;
  verbose_ = verbose;
  wrpp_ = wrpp;
  pcp_ = pcp;
}

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

  if(search_radius_<5)
    {
      msg << "ERROR: unrealistic to use a search radius< 5";
      valid = false;
    }

  valid = valid && wrpp_.SanityCheck();
  msg << wrpp_.GetErrorMsg() << vcl_ends;
  
  valid = valid && pcp_.SanityCheck();
  msg << pcp_.GetErrorMsg() << vcl_ends;
  
  SetErrorMsg(msg.str().c_str());
  return valid;
}

 vcl_ostream& operator << (vcl_ostream& os, const sdet_vehicle_finder_params& vfp)
{
  os << "sdet_vehicle_finder_params:" << vcl_endl << "[---" << vcl_endl;
  os << vfp.wrpp_ << vcl_endl;
  os << vfp.pcp_ << vcl_endl;
     os << "  ---" << vcl_endl;
  os << "debug " << vfp.debug_ << vcl_endl;
  os << "verbose " << vfp.verbose_ << vcl_endl;
  os << "search radius " << vfp.search_radius_ << vcl_endl;
  os << "shadow thresh " << vfp.shadow_thresh_ << vcl_endl;
  os << "distance scale " << vfp.distance_scale_ << vcl_endl;
  os << "para cvrg thresh " << vfp.para_thresh_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
