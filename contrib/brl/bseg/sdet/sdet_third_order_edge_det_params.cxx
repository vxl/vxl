// This is brl/bseg/sdet/sdet_third_order_edge_det_params.cxx
#include "sdet_third_order_edge_det_params.h"
//:
// \file
// See sdet_third_order_edge_det_params.h
//
//-----------------------------------------------------------------------------
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//------------------------------------------------------------------------
// Constructors
//

sdet_third_order_edge_det_params::
sdet_third_order_edge_det_params(const sdet_third_order_edge_det_params& dp)
  : gevd_param_mixin()
{
  InitParams(dp.sigma_, dp.thresh_, dp.interp_factor_, dp.pfit_type_,
             dp.grad_op_, dp.conv_algo_,dp.adapt_thresh_);
}

sdet_third_order_edge_det_params::
sdet_third_order_edge_det_params(const double sigma, const double thresh,
                                 const unsigned interp_factor, 
                                 const unsigned pfit_type, 
                                 const unsigned grad_op,
                                 const unsigned conv_algo,
                                 const bool adapt_thresh)
{
  InitParams(sigma, thresh, interp_factor, pfit_type, grad_op, conv_algo, adapt_thresh);
}

void sdet_third_order_edge_det_params::InitParams(double sigma, double thresh,
                                                  unsigned interp_factor,
                                                  unsigned pfit_type,
                                                  unsigned grad_op,
                                                  unsigned conv_algo,
                                                  bool adapt_thresh)
{
  sigma_ = sigma,
  thresh_ = thresh;
  interp_factor_ = interp_factor;
  pfit_type_ = pfit_type;
  grad_op_ = grad_op;
  conv_algo_ = conv_algo;
  adapt_thresh_ = adapt_thresh;
}

//-----------------------------------------------------------------------------
//
//:   Checks that parameters are within acceptable bounds
//    Note that msg << ends seems to restart the string and erase the
//    previous string. We should only use it as the last call, use
//    vcl_endl otherwise.
bool sdet_third_order_edge_det_params::SanityCheck()
{
  vcl_stringstream msg;
  bool valid = true;

  if (thresh_<0 || thresh_>10)
  {
    msg << "ERROR: percentage threshold should be between 0 and 10";
    valid = false;
  }
  msg << vcl_ends;

  SetErrorMsg(msg.str().c_str());
  return valid;
}

vcl_ostream& operator<< (vcl_ostream& os, const sdet_third_order_edge_det_params& dp)
{
  return
  os << "sdet_third_order_edge_det_params:\n[---\n"
     << "Smooth sigma " << dp.sigma_ << vcl_endl
     << "Gradient threshold in percentage " << dp.thresh_ << vcl_endl
     << "---]" << vcl_endl;
}
