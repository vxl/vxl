// This is mul/vil2/algo/vil2_gauss_filter.cxx
#include "vil2_gauss_filter.h"
//:
// \file
// \brief Functions to smooth an image
// \author Ian Scott
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vnl/vnl_gamma.h> // for vnl_erf()

vil2_gauss_filter_5tap_params::vil2_gauss_filter_5tap_params(double sigma)
{
  sigma_ = sigma;
  const double z = 1/(vcl_sqrt(2.0)*sigma);
  filt0_ = vnl_erf(0.5 * z) - vnl_erf(-0.5 * z);
  filt1_ = vnl_erf(1.5 * z) - vnl_erf(0.5 * z);
  filt2_ = vnl_erf(2.5 * z) - vnl_erf(1.5 * z);

  double five_tap_total = 2*(filt2_ + filt1_) + filt0_;
//  double four_tap_total = filt2_ + 2*(filt1_) + filt0_;
//  double three_tap_total = filt2_ + filt1_ + filt0_;

//  Calculate 3 tap half Gaussian filter assuming constant edge extension
  filt_edge0_ = (filt0_ + filt1_ + filt2_) / five_tap_total;
  filt_edge1_ = filt1_ / five_tap_total;
  filt_edge2_ = filt2_ / five_tap_total;
#if 0
  filt_edge0_ = 1.0;
  filt_edge1_ = 0.0;
  filt_edge2_ = 0.0;
#endif
//  Calculate 4 tap skewed Gaussian filter assuming constant edge extension
  filt_pen_edge_n1_ = (filt1_+filt2_) / five_tap_total;
  filt_pen_edge0_ = filt0_ / five_tap_total;
  filt_pen_edge1_ = filt1_ / five_tap_total;
  filt_pen_edge2_ = filt2_ / five_tap_total;

//  Calculate 5 tap Gaussian filter
  filt0_ = filt0_ / five_tap_total;
  filt1_ = filt1_ / five_tap_total;
  filt2_ = filt2_ / five_tap_total;

  assert(filt_edge0_ > filt_edge1_);
  assert(filt_edge1_ > filt_edge2_);
}
