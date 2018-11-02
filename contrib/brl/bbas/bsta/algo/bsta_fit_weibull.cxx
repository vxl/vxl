// This is brl/bbas/bsta/algo/bsta_fit_weibull.cxx
#ifndef bsta_fit_weibull_cxx_
#define bsta_fit_weibull_cxx_
//:
// \file
#include "bsta_fit_weibull.h"
#include <vnl/vnl_gamma.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//this function computes the model value for (mean/standard_dev)^2 as
//a function of Weibull parameter k.
static double mean_sigma_sq(double k)
{
  double m = vnl_gamma((1+k)/k);
  m *=m;
  double den = vnl_gamma((2+k)/k)-m;
  return m/den;
}
//The cost function is ((mean/sigma)^2(k) - sample_mean^2/sample_variance)^2
double bsta_weibull_cost_function::f(vnl_vector<double> const& x)
{
  double k = x[0];
  assert(k>0);
  double r = mean_/std_dev_;
  r *= r;
  double r_weib = mean_sigma_sq(k);
  double ret = r_weib-r;
  return ret*ret;
}

void bsta_weibull_cost_function::gradf(vnl_vector<double> const& x,
                                       vnl_vector<double>& gradient)
{
  double k = x[0];
  assert(k>0);
  double m = vnl_gamma((1+k)/k);
  m*=m;
  double v = vnl_gamma((2+k)/k);
  double tn = vnl_digamma((2+k)/k)-vnl_digamma((1+k)/k);
  double neu = 2.0*m*v*tn;
  double den = k*k*(m-v)*(m-v);
  double r = mean_/std_dev_;
  r *= r;
  double r_weib = mean_sigma_sq(k);
  double res = r_weib-r;
  gradient[0]=2*res*neu/den;
}

double bsta_weibull_cost_function::lambda(double k) const
{
  assert(k>0);
  return mean_/vnl_gamma((1+k)/k);
}
#endif // bsta_fit_weibull_cxx_
