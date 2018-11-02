// This is brl/bseg/sdet/sdet_third_order_edge_det_params.h
#ifndef sdet_third_order_edge_det_params_h_
#define sdet_third_order_edge_det_params_h_
//:
// \file
// \brief parameter mixin for third order edge detection
//
// \author
//    J. Mundy - July 22, 2010
//    Brown University
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


class sdet_third_order_edge_det_params : public gevd_param_mixin
{
 public:
// enumeration for the parabola fit type
  enum {PFIT_3_POINTS, PFIT_9_POINTS};

  sdet_third_order_edge_det_params(const double sigma=1.0, const double thresh = 2.0, const unsigned interp_factor = 1, const unsigned pfit_type = 0, const unsigned grad_op = 0, const unsigned conv_algo=0, const bool adapt_thresh = false);

  sdet_third_order_edge_det_params(const sdet_third_order_edge_det_params& old_params);
  ~sdet_third_order_edge_det_params() override= default;

  bool SanityCheck() override;
  friend
    std::ostream& operator<<(std::ostream&,const sdet_third_order_edge_det_params& dp);
 protected:
  void InitParams(double sigma, double thresh, unsigned interp_factor,
                  unsigned pfit_type, unsigned grad_op, unsigned conv_algo, bool adapt_thresh);
 public:
  //: points with gradient magnitude below thresh_*maximum_gradient_magnitude/100 will not be processed.
  double sigma_;
  double thresh_;
  unsigned interp_factor_;
  unsigned pfit_type_;
  unsigned grad_op_;
  unsigned conv_algo_;
  bool adapt_thresh_;
};

#endif // sdet_third_order_edge_det_params_h_
