// This is gel/gevd/gevd_region_proc_params.cxx
#include <iostream>
#include <sstream>
#include "gevd_region_proc_params.h"
//:
// \file
// See gevd_region_proc_params.h
//
//-----------------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//------------------------------------------------------------------------
// Constructors
//

gevd_region_proc_params::
gevd_region_proc_params(const gevd_region_proc_params& rpp)
  : gevd_param_mixin()
{
  InitParams(rpp.expand_scale_,
             rpp.burt_adelson_factor_,
             rpp.debug_,
             rpp.verbose_,
             (gevd_detector_params)rpp.dp_
            );
}

gevd_region_proc_params::gevd_region_proc_params(float expand_scale,
                                       float burt_adelson_factor,
                                       bool debug,
                                       bool verbose,
                                       const gevd_detector_params& dp
                                       )
{
  InitParams(expand_scale, burt_adelson_factor, debug, verbose, dp);
}

void gevd_region_proc_params::InitParams(float expand_scale,
                                    float burt_adelson_factor,
                                    bool debug,
                                    bool verbose,
                                    const gevd_detector_params& dp)
{
  expand_scale_ = expand_scale;
  burt_adelson_factor_ = burt_adelson_factor;
  debug_ = debug;
  verbose_ = verbose;
  dp_ = dp;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool gevd_region_proc_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;

  if (expand_scale_ <0.5 || expand_scale_ >2) {
    msg << "currently only handle a scale factor of 0.5, 1.0, or 2.0\n";
    valid = false;
  }

  if (burt_adelson_factor_ > .6 || burt_adelson_factor_ <.3) {
    msg << "burt_adelson_factor must be in the range .3<=ka<=.6\n";
    valid = false;
  }

  valid = valid && dp_.SanityCheck();
  msg << dp_.GetErrorMsg() << std::ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const gevd_region_proc_params& rpp)
{
  return
  os << "gevd_region_proc_params:\n[---\n"
  // << rpp.dp_ << "\n  ---\n"
     << "expand scale factor  " << rpp.expand_scale_ << std::endl
     << "burt_adelson_factor " << rpp.burt_adelson_factor_ << std::endl
     << "debug " << rpp.debug_ << std::endl
     << "verbose " << rpp.verbose_ << std::endl
     << "---]" << std::endl;
}
