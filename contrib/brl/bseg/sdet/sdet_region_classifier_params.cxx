// This is brl/bseg/sdet/sdet_region_classifier_params.cxx
#include <iostream>
#include <sstream>
#include "sdet_region_classifier_params.h"
//:
// \file
// See sdet_region_classifier_params.h
//
//-----------------------------------------------------------------------------

#include <vcl_compiler.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_region_classifier_params::
sdet_region_classifier_params(const sdet_region_classifier_params& rpp)
  : gevd_param_mixin()
{
  InitParams(rpp.use_vd_edges_,
             rpp.sigma_,
             rpp.vd_noise_mul_,
             rpp.four_or_eight_conn_,
             rpp. margin_,
             rpp.weight_thresh_,
             rpp.min_region_size_,
             rpp.nbins_,
             rpp.verbose_,
             rpp.debug_
            );
}

sdet_region_classifier_params::
sdet_region_classifier_params(bool use_vd_edges,
                             float sigma,
                                     float vd_noise_mul,
                                     int four_or_eight_conn,
                                     unsigned margin,
                                     double weight_thresh,
                                     int min_region_size,
                                     unsigned nbins,
                                     bool verbose,
                                     bool debug
                                     )
{
  InitParams(use_vd_edges,sigma, vd_noise_mul, four_or_eight_conn, margin, weight_thresh,
             min_region_size, nbins, verbose, debug);
}

void sdet_region_classifier_params::InitParams(bool use_vd_edges,
                                              float sigma,
                                                      float vd_noise_mul,
                                                      int four_or_eight_conn,
                                                      unsigned margin,
                                                      double weight_thresh,
                                                      int min_region_size,
                                                      unsigned nbins,
                                                      bool verbose,
                                                      bool debug)
{
  use_vd_edges_ = use_vd_edges;
  sigma_ = sigma;
  vd_noise_mul_ = vd_noise_mul;
  four_or_eight_conn_ = four_or_eight_conn;
   margin_ =  margin;
  weight_thresh_ = weight_thresh;
  min_region_size_ = min_region_size;
  nbins_ = nbins;
  debug_ = debug;
  verbose_ = verbose;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    std::endl otherwise.
bool sdet_region_classifier_params::SanityCheck()
{
  std::stringstream msg;
  bool valid = true;
  SetErrorMsg(msg.str().c_str());
  return valid;
}

std::ostream& operator << (std::ostream& os, const sdet_region_classifier_params& rpp)
{
  return
  os << "sdet_region_classifier_params:\n[---\n"
     << "use vd edges " << rpp.use_vd_edges_ << std::endl
     << "sigma " << rpp.sigma_ << std::endl
     << "noise mul " << rpp.vd_noise_mul_ << std::endl
     << "four_or_eight_conn " << rpp.four_or_eight_conn_ << std::endl
     << "margin " << rpp.margin_ << std::endl
     << "weight thresh " << rpp.weight_thresh_ << std::endl
     << "min_region_size " << rpp.min_region_size_ << std::endl
     << "n_hist_bins " << rpp.min_region_size_ << std::endl
     << "debug " << rpp.debug_ << std::endl
     << "verbose " << rpp.verbose_ << std::endl
     << "---]" << std::endl;
}
