// This is brl/bseg/sdet/sdet_region_proc_params.cxx
#include <sdet/sdet_region_proc_params.h>
//:
// \file
// See sdet_region_proc_params.h
//
//-----------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_region_proc_params::
sdet_region_proc_params(const sdet_region_proc_params& rpp)
{
  InitParams((sdet_detector_params)rpp.dp_,
             rpp.verbose_,
             rpp.debug_,
             rpp.array_scale_
            );
}

sdet_region_proc_params::
sdet_region_proc_params(const sdet_detector_params& dp,
                        bool verbose,
                        bool debug,
                        int array_scale
                        )
{
  InitParams(dp, verbose, debug, array_scale);
}

void sdet_region_proc_params::InitParams(const sdet_detector_params& dp,
                                         bool verbose,
                                         bool debug,
                                         int array_scale)
{
  array_scale_ = array_scale;
  debug_ = debug;
  verbose_ = verbose;
  dp_ = dp;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_region_proc_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if(array_scale_<2)
    {
      msg << "ERROR: don't use a label array resolution factor less than 2x";
      valid = false;
    }

  valid = valid && dp_.SanityCheck();
  msg << dp_.GetErrorMsg() << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const sdet_region_proc_params& rpp)
{
  os << "sdet_region_proc_params:" << vcl_endl << "[---" << vcl_endl;
  os << rpp.dp_ << vcl_endl
     << "  ---" << vcl_endl;
  os << "debug " << rpp.debug_ << vcl_endl;
  os << "verbose " << rpp.verbose_ << vcl_endl;
  os << "label array scale " << rpp.array_scale_ << vcl_endl;
  os << "---]" << vcl_endl;
  return os;
}
