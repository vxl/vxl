// This is brl/bseg/sdet/sdet_denoise_mrf_bp_params.h
#ifndef sdet_denoise_mrf_bp_params_h_
#define sdet_denoise_mrf_bp_params_h_
//:
// \file
// \brief parameter mixin for sdet_denoise_mrf_bp
//
// \author
//    Joseph L. Mundy - March 30, 2011
//    Brown University
//
//-----------------------------------------------------------------------------
#include <iostream>
#include <iosfwd>
#include <vbl/vbl_ref_count.h>
#include <gevd/gevd_param_mixin.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class sdet_denoise_mrf_bp_params : public gevd_param_mixin, public vbl_ref_count
{
 public:

  sdet_denoise_mrf_bp_params(unsigned n_labels = 256,
                             unsigned n_iter = 5, unsigned pyramid_levels = 5,
                             float discontinuity_cost = 200.0f,
                             float truncation_cost = 10000.0f,
                             float kappa= 1.0,
                             float lambda = 0.05f);

  sdet_denoise_mrf_bp_params(const sdet_denoise_mrf_bp_params& old_params);
 ~sdet_denoise_mrf_bp_params() override = default;

  bool SanityCheck() override;
 friend
  std::ostream& operator<<(std::ostream& os, const sdet_denoise_mrf_bp_params& imp);
 protected:
  void InitParams(unsigned n_labels, unsigned n_iter, unsigned pyramid_levels,
                  float discontinuity_cost, float truncation_cost,
                  float kappa, float lambda);
 public:
  //
  // Parameter blocks and parameters
  //
  unsigned n_labels_;
  unsigned n_iter_;
  unsigned pyramid_levels_;
  float discontinuity_cost_;
  float truncation_cost_;
  float kappa_;
  float lambda_;
};

#endif // sdet_denoise_mrf_bp_params_h_
