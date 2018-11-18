// This is rpl/rrel/rrel_kernel_density_obj.cxx
#include <iostream>
#include <algorithm>
#include "rrel_kernel_density_obj.h"
#include <rrel/rrel_muset_obj.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

namespace {
  inline void shft2(double &a, double &b, const double c)
  {
    a = b;
    b = c;
  }

  inline void shft3(double &a, double &b, double &c, const double d)
  {
    a = b;
    b = c;
    c = d;
  }
}

rrel_kernel_density_obj::rrel_kernel_density_obj(rrel_kernel_scale_type scale_type)
  : scale_type_(scale_type),
    fix_x_( false )
{
}

double
rrel_kernel_density_obj::fcn(vect_const_iter /*res_begin*/, vect_const_iter /*res_end*/,
                             vect_const_iter /*scale_begin*/,
                             vnl_vector<double>* /*param_vector*/ ) const
{
  std::cerr << "rrel_kernel_density_obj::fcn() not implemented\n";
  return 0;
}

double
rrel_kernel_density_obj::fcn(vect_const_iter res_begin,
                             vect_const_iter res_end,
                             double prior_scale,
                             vnl_vector<double>* ) const
{
  double h = bandwidth ( res_begin, res_end, prior_scale );
  assert ( h != 0 );
  double x = best_x ( res_begin, res_end, prior_scale );

  return -1 * kernel_density( res_begin, res_end, x, h );
}

double
rrel_kernel_density_obj::best_x(vect_const_iter res_begin,
                                vect_const_iter res_end,
                                double prior_scale) const
{
  if (fix_x_)
    return 0;

  //Golden Section Search is adapted from "Numerical Recipes in C++"
  //to find x that maximizes kernel_density.
  const double R = 0.61803399, C = 1.0 - R; //The golden ratios.
  double f1, f2, x0, x1, x2, x3;
  double tol = 1.0e-9;
  double f0 = 0;

  double h = bandwidth(res_begin, res_end, prior_scale);
  assert(h!=0);

  std::vector<double> sort_res( res_begin, res_end );
  std::sort( sort_res.begin(), sort_res.end() );

  unsigned int loc = 0;
  unsigned int i = 0;
  for ( ; i<sort_res.size(); ++i ) {
    double x = sort_res[i];
    double f = kernel_density( res_begin, res_end, x, h );
    if (f > f0) {
      f0 = f;
      loc = i;
    }
  }

  x0 = sort_res[loc-1];
  x3 = sort_res[loc+1];

  if ( vnl_math::abs( x3 - sort_res[loc] ) > vnl_math::abs( sort_res[loc] - x0 ) ) {
    x1 = sort_res[loc];
    x2 = sort_res[loc] + C * ( x3 - sort_res[loc] );
  }
  else {
    x2 = sort_res[loc];
    x1 = sort_res[loc] - C * ( sort_res[loc] - x0 );
  }

  f1 = kernel_density( res_begin, res_end, x1, h );
  f2 = kernel_density( res_begin, res_end, x2, h );
  while ( vnl_math::abs( x3 - x0 ) >
          tol * vnl_math::abs( x1 ) + vnl_math::abs( x2 ) ) {
    if ( f2 > f1 ) {
      shft3( x0, x1, x2, R * x2 + C * x3 );
      shft2( f1, f2, kernel_density( res_begin, res_end, x2, h ) );
    }
    else {
      shft3( x3, x2, x1, R * x1 + C * x0 );
      shft2( f2, f1, kernel_density( res_begin, res_end, x1, h ) );
    }
  }
  if (f1 < f2)
    return x2;
  else
    return x1;
}

double
rrel_kernel_density_obj::bandwidth(vect_const_iter res_begin, vect_const_iter res_end,
                                   double prior_scale) const
{
  std::vector<double>::difference_type n = res_end - res_begin;
  double scale = 1.0;

  switch ( scale_type_ )
  {
   case RREL_KERNEL_MAD: {
    //A median absolute deviations (MAD) scale estimate.

    //Here I avoid using rrel_util_median_abs_dev_scale
    //because it assumes residuals are zero-based and have a Gaussian distribution.

    std::vector<double> residuals(res_begin, res_end);
    auto loc = residuals.begin() + n / 2;
    std::nth_element( residuals.begin(), loc, residuals.end() );

    double res_median = *loc;
    std::vector<double> abs_res_median;
    abs_res_median.reserve( n );
    for (std::vector<double>::difference_type i=0; i<n; ++i ) {
      abs_res_median.push_back( vnl_math::abs( residuals[i] - res_median ) );
    }
    loc = abs_res_median.begin() + n / 2;
    std::nth_element( abs_res_median.begin(), loc, abs_res_median.end() );
    // c=0.5 is chosen to avoid over-smoothing of the estimated density.
    scale = 0.5 * (*loc);
    break;
   }

   case RREL_KERNEL_PRIOR:
    scale = prior_scale;
    break;

   case RREL_KERNEL_MUSE: {
    rrel_muset_obj muse( (int)n );
    scale = muse.fcn( res_begin, res_end );
    break; }

   default:
    assert(!"invalid scale_type");
  }

  // h = [243 * R(K) / 35 / Mu(K)^2 / n]^0.2 * scale
  // R(K) = Integral ( K(u)^2 ) du
  // Mu(K) = Integral ( u^2 * K(u) ) du
  const double c = 65610.0 / 143;
  return std::pow( c / n , 0.2 ) * scale;
}

double
rrel_kernel_density_obj::kernel_density(vect_const_iter res_begin,
                                        vect_const_iter res_end,
                                        double x,
                                        double h) const
{
  double f=0;
  std::vector<double>::difference_type n = res_end - res_begin;
  for ( ; res_begin!= res_end; ++res_begin ) {
    f += kernel_function( ( *res_begin - x ) / h );
  }
  f /= n*h;
  return f;
}

double
rrel_kernel_density_obj::kernel_function(double u) const
{
  if (vnl_math::abs(u) > 1)
    return 0;

  double t = 1 - u * u;
  return 1.09375 * t * t * t;
}
