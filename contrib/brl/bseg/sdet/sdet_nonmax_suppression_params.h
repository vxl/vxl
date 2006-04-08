// This is brl/bseg/sdet/sdet_nonmax_suppression_params.h
#ifndef sdet_nonmax_suppression_params_h_
#define sdet_nonmax_suppression_params_h_
//:
// \file
// \brief parameter mixin for sdet_nonmax_suppression
//
// \author
//    H. Can Aras - Feb. 20, 2006
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class sdet_nonmax_suppression_params : public gevd_param_mixin
{
 public:
  sdet_nonmax_suppression_params(const double thresh = 50);

  sdet_nonmax_suppression_params(const sdet_nonmax_suppression_params& old_params);
  ~sdet_nonmax_suppression_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&,const sdet_nonmax_suppression_params& dp);
 protected:
  void InitParams(double thresh);
 public:
  //: points with gradient magnitude below thresh_*maximum_gradient_magnitude/100 will not be processed.
  double thresh_;
};

#endif // sdet_nonmax_suppression_params_h_
