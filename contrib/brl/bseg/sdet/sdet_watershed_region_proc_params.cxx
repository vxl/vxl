// This is brl/bseg/sdet/sdet_watershed_region_proc_params.cxx
#include <sdet/sdet_watershed_region_proc_params.h>
//:
// \file
// See sdet_watershed_region_proc_params.h
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_watershed_region_proc_params::
sdet_watershed_region_proc_params(const sdet_watershed_region_proc_params& wrpp)
{
  InitParams((brip_watershed_params)wrpp.wp_,
             wrpp.verbose_,
             wrpp.debug_,
             wrpp.min_area_,
             wrpp.merge_tol_,
             wrpp.merge_priority_
             );
}

sdet_watershed_region_proc_params::
sdet_watershed_region_proc_params(const brip_watershed_params& wp,
                                  bool verbose,
                                  bool debug,
                                  int min_area,
                                  float merge_tol,
                                  int merge_priority
                                  )
{
  InitParams(wp, verbose, debug, min_area, merge_tol, merge_priority);
}

void sdet_watershed_region_proc_params::InitParams(const brip_watershed_params& wp,
                                                   bool verbose,
                                                   bool debug,
                                                   int min_area,
                                                   float merge_tol,
                                                   int merge_priority)
{
  min_area_ = min_area;
  merge_tol_ = merge_tol;
  merge_priority_ = merge_priority;
  debug_ = debug;
  verbose_ = verbose;
  wp_ = wp;
}

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

  if(min_area_<2)
    {
      msg << "ERROR: don't use a label array resolution factor less than 2x";
      valid = false;
    }

  valid = valid && wp_.SanityCheck();
  msg << wp_.GetErrorMsg() << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_watershed_region_proc_params& wrpp)
{
  os << "sdet_watershed_region_proc_params:" << vcl_endl << "[---" << vcl_endl;
  os << wrpp.wp_ << vcl_endl
     << "  ---" << vcl_endl;
  os << "debug " << wrpp.debug_ << vcl_endl;
  os << "verbose " << wrpp.verbose_ << vcl_endl;
  os << "min region area " << wrpp.min_area_ << vcl_endl;
  os << "region merge tol " << wrpp.merge_tol_ << vcl_endl;
  os << "merge priority " << wrpp.merge_priority_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
