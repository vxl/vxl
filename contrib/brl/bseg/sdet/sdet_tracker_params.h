// This is brl/bseg/sdet/sdet_tracker_params.h
#ifndef sdet_tracker_params_h_
#define sdet_tracker_params_h_
//:
// \file
// \brief parameter mixin for sdet_tracker
//
// \author
//    Joseph L. Mundy - August. 20, 2003
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class sdet_tracker_params : public gevd_param_mixin
{
 public:
  sdet_tracker_params(const int n_samples = 10, 
                      const float search_radius = 5.0,
                      const float angle_range = 0.0,
                      const float scale_range = 0.0,
                      const float sigma = 1.0,
                      const bool gradient_info = false
                      );

 sdet_tracker_params(const sdet_tracker_params& old_params);
 ~sdet_tracker_params(){}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const sdet_tracker_params& tp);
 protected:
 void InitParams(int n_samples, 
                 float search_radius,
                 float angle_range,
                 float scale_range,
                 float sigma,
                 bool gradient_info
                 );
 public:
  //
  // Parameter blocks and parameters
  //
  int n_samples_;       // number of samples generated per iteration
  float search_radius_; // window size for generating samples
  float angle_range_;   // angle range for generating samples
  float scale_range_;   // scale range for generating samples
  float sigma_;         // Gaussian smoothing kernel stddev for estimating gradient
  bool gradient_info_;  // Combine gradient and intensity info
};

#endif // sdet_tracker_params_h_
