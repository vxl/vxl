// This is brl/bseg/strk/strk_region_info_params.cxx
#include <strk/strk_region_info_params.h>
//:
// \file
// See strk_region_info_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>

//------------------------------------------------------------------------
// Constructors
//

strk_region_info_params::
strk_region_info_params(const strk_region_info_params& tp)
  : gevd_param_mixin()
{
  InitParams(tp.sigma_,
             tp.gradient_info_,
             tp.color_info_,
             tp.min_gradient_,
             tp.parzen_sigma_,
             tp.verbose_,
             tp.debug_
             );
}

strk_region_info_params::
strk_region_info_params( const float sigma,
                         const bool gradient_info,
                         const bool color_info,
                         const float min_gradient,
                         const float parzen_sigma,
                         const bool verbose,
                         const bool debug)
{
  InitParams(sigma,
             gradient_info,
             color_info,
             min_gradient,
             parzen_sigma,
             verbose,
             debug);
}

void strk_region_info_params::InitParams( float sigma,
                                          bool gradient_info,
                                          bool color_info,
                                          float min_gradient,
                                          float parzen_sigma,
                                          bool verbose,
                                          bool debug)
{
  sigma_ = sigma;
  gradient_info_ = gradient_info;
  color_info_ = color_info;
  min_gradient_ = min_gradient;
  parzen_sigma_ = parzen_sigma;
  verbose_ = verbose;
  debug_ = debug;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool strk_region_info_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

   msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator << (vcl_ostream& os, const strk_region_info_params& tp)
{
  return
  os << "strk_region_info_params:\n[---\n"
     << "sigma " << tp.sigma_ << vcl_endl
     << "gradient_info " << tp.gradient_info_ << vcl_endl
     << "color_info " << tp.color_info_ << vcl_endl
     << "min_gradient " << tp.min_gradient_ << vcl_endl
     << "parzen_sigma " << tp.parzen_sigma_ << vcl_endl
     << "verbose " << tp.verbose_ << vcl_endl
     << "debug " << tp.debug_ << vcl_endl
     << "---]" << vcl_endl;
}
