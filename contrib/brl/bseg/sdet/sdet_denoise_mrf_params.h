// This is brl/bseg/sdet/sdet_denoise_mrf_params.h
#ifndef sdet_denoise_mrf_params_h_
#define sdet_denoise_mrf_params_h_
//:
// \file
// \brief parameter mixin for sdet_denoise_mrf
//
// \author
//    Joseph L. Mundy - March 22, 2011
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

class sdet_denoise_mrf_params : public gevd_param_mixin, public vbl_ref_count
{
 public:
  sdet_denoise_mrf_params(double radius=1.0, double kappa= 1.0,
                          double beta = 1.0 );


  sdet_denoise_mrf_params(const sdet_denoise_mrf_params& old_params);
 ~sdet_denoise_mrf_params() override = default;

  bool SanityCheck() override;
 friend
  std::ostream& operator<<(std::ostream& os, const sdet_denoise_mrf_params& imp);
 protected:
  void InitParams(double radius, double kappa, double beta);
 public:
  //
  // Parameter blocks and parameters
  //
  double radius_; //!< MRF neighborhood radius
  double kappa_; //!< parameter for variance weight
  double beta_; //!< parameter for height difference weight
};

#endif // sdet_denoise_mrf_params_h_
