//--*-c++-*-
// This is brl/bseg/strk/strk_region_info_params.h
#ifndef strk_region_info_params_h_
#define strk_region_info_params_h_
//:
// \file
// \brief parameter mixin for strk_region_info
//
// \author
//    Joseph L. Mundy - March 14, 2004
//    Brown University
//
//-----------------------------------------------------------------------------
#include <gevd/gevd_param_mixin.h>
#include <vcl_iostream.h>

class strk_region_info_params : public gevd_param_mixin
{
 public:
  strk_region_info_params(const float sigma = 1.0,
                          const bool gradient_info = true,
                          const bool color_info = false,
                          const float min_gradient = 0,
                          const float parzen_sigma = 0,
                          const bool verbose = false,
                          const bool debug = false
                          );

 strk_region_info_params(const strk_region_info_params& old_params);
 ~strk_region_info_params() {}

  bool SanityCheck();
 friend
  vcl_ostream& operator<<(vcl_ostream& os, const strk_region_info_params& tp);
 protected:
 void InitParams(float sigma,
                 bool gradient_info,
                 bool color_info,
                 float min_gradient,
                 float parzen_sigma,
                 bool verbose,
                 bool debug
                );
 public:
  //
  // Parameter blocks and parameters
  //
  float sigma_;            //!< smoothing kernel radius for estimating gradient
  bool gradient_info_;     //!< Combine gradient and intensity info
  bool color_info_;        //!< Combine color and intensity info
  float min_gradient_;     //!< minimum gradient magnitude to be considered
  float parzen_sigma_;     //!< smoothing for the histogram density
  bool verbose_;           //!< informative messages to cout
  bool debug_;             //!< informative debug messages to cout
};

#endif // strk_region_info_params_h_
