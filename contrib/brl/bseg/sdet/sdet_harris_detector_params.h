// This is brl/bseg/sdet/sdet_harris_detector_params.h
#ifndef sdet_harris_detector_params_h_
#define sdet_harris_detector_params_h_
//:
// \file
// \brief parameter mixin for sdet_harris_detector
//
// \author
//    Joseph L. Mundy - Feb. 26, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iosfwd.h>

class sdet_harris_detector_params : public gevd_param_mixin
{
 public:
  sdet_harris_detector_params(const float sigma=1.0,
                              const float thresh = 2.0,
                              const int n=1,
                              const float percent_corners=80.0,
                              const float scale_factor = 0.04);

  sdet_harris_detector_params(const sdet_harris_detector_params& old_params);
 ~sdet_harris_detector_params(){}

  bool SanityCheck();
  friend
    vcl_ostream& operator<<(vcl_ostream&,const sdet_harris_detector_params& dp);
 protected:
  void InitParams(float sigma,
                  float thresh,
                  int n,
                  float percent_corners,
                  float scale_factor);
 public:
  //
  // Parameter blocks and parameters
  //
  float sigma_;       //!< sigma of the Gaussian smoothing kernel
  float thresh_;      //!< threshold for weak corners
  int n_;             //!< size of the gradient matrix neighorhood (2n+1)x(2n+1)
  float percent_corners_; //!< maximum of corners as a percent of total
  float scale_factor_;    //!< Harris trace weight
};

#endif // sdet_harris_detector_params_h_
