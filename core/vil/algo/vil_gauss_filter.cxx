// This is core/vil/algo/vil_gauss_filter.cxx
#include "vil_gauss_filter.h"
//:
// \file
// \brief Functions to smooth an image
// \author Ian Scott
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vcl_functional.h>
#include <vnl/vnl_erf.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_real_polynomial.h>

vil_gauss_filter_5tap_params::vil_gauss_filter_5tap_params(double sigma)
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


//: Generate an n-tap FIR filter from a Gaussian function.
// The filter uses the equation $k D^d \exp -\frac{x^2}{2\sigma^2} $,
// where D is the differential operator, and k is a normalising constant.
// \param diff The number of differential operators to apply to the filter.
// If you want just a normal gaussian, set diff to 0.
// \param sd The width of the gaussian.
//
// The taps will be calculated using the itegeral of the above equation over
// the pixel width. However, aliasing will reduce the meaningfulness of
// your filter when sd << (diff+1). In most applications you will
// want filter.size() ~= sd*7, which will avoid significant truncation,
// without wasting the outer taps on near-zero values.
void vil_gauss_filter_gen_ntap(double sd, unsigned diff,
                               vcl_vector<double> &filter)
{
  unsigned centre = filter.size()/2; // or just past centre if even length
  double sum=0.0; // area under sampled curve.
  double tap; // workspace

  if (diff==0)
  {
    const double z = 1/(vcl_sqrt(2.0)*sd);
    if (filter.size() % 2 == 0)  // even length filter - off-centre
    {
      for (unsigned i=0 ; i<centre; ++i)
      {
        tap = vnl_erf((i+1.0) * z) - vnl_erf(i * z);
        sum += tap;
        filter[centre+i] = filter[centre-i-1] = tap;
      }
      sum *= 2.0;
    }
    else // odd length filter - centre on zero
    {
      for (unsigned i=1 ; i<=centre; ++i)
      {
        tap = vnl_erf((i+0.5) * z) - vnl_erf((i-0.5) * z);
        sum += tap;
        filter[centre+i] = filter[centre-i] = tap;
      }
      sum *= 2.0;
      tap = vnl_erf(0.5 * z) - vnl_erf(-0.5 * z);
      sum += tap;
      filter[centre] = tap;
    }
  }
  else
  {
    const double offset = filter.size() % 2 == 0 ? 0.0 : -0.5;
    vnl_real_polynomial poly(1.0);
    const double eta = -0.5/(sd*sd);
    const vnl_real_polynomial d_gauss(vnl_vector<double>(2, eta, 0.0));
    for (unsigned i=1; i<diff; ++i)
      // Evaluate d/dx (poly * gauss) where gauss = exp(-0.5*x^2/sd^2)
      // n.b. d/dx gauss = d_gauss * gauss
      poly = poly * d_gauss + poly.derivative();

    for (int i=-centre ; i+centre<filter.size(); ++i)
    {
      tap = poly.evaluate(i+1.0+offset)*vcl_exp(eta*(i+1.0+offset)*(i+1.0+offset))
          - poly.evaluate(i+    offset)*vcl_exp(eta*(i+    offset)*(i+    offset));
      sum += vcl_abs(tap);
      filter[centre+i] = tap;
    }
  }

  // normalise the result
  assert(sum >= 0.0);
  double norm = 1.0 / sum;
  vcl_transform(filter.begin(), filter.end(), filter.begin(),
                vcl_bind2nd(vcl_multiplies<double>(), norm));
}
