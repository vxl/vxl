#include <rrel/rrel_kernel_density_obj.h>
#include <rrel/rrel_muset_obj.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

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

rrel_kernel_density_obj::rrel_kernel_density_obj(rrel_kernel_scale_type scale_type)
  : scale_type_(scale_type)
{
}

double
rrel_kernel_density_obj::fcn( vect_const_iter res_begin, vect_const_iter res_end,
                             vect_const_iter scale_begin,
                             vnl_vector<double>* param_vector ) const
{
  //not implemented.
  return 0;
}

double
rrel_kernel_density_obj::fcn(vect_const_iter res_begin, vect_const_iter res_end,
                             double prior_scale,
                             vnl_vector<double>* ) const
{
  unsigned n = res_end - res_begin;
  double h = bandwidth ( res_begin, res_end, prior_scale ); 
  assert(h!=0);
  double f = 0;

  double x = best_x ( res_begin, res_end, prior_scale );

  f = kernel_density( res_begin, res_end, x, h );
  return -f;
}

double
rrel_kernel_density_obj::best_x(vect_const_iter res_begin, vect_const_iter res_end,
                                double prior_scale) const
{
  const double R = 0.61803399, C = 1.0 - R; //The golden ratios.
  double f1, f2, x0, x1, x2, x3;
  double tol = 1.0e-9;

  unsigned int n = res_end - res_begin;
  double h = bandwidth(res_begin, res_end, prior_scale); 
  assert(h!=0);

  vect_const_iter mloc = vcl_min_element(res_begin, res_end);
  double min = *mloc - h;
  mloc = vcl_max_element(res_begin, res_end);
  double max = *mloc + h;
  vcl_vector<double> res(res_begin, res_end);
  vcl_vector<double>::iterator loc = res.begin() + n / 2;
  vcl_nth_element(res.begin(), loc, res.end());
  double median = *loc;

  x0 = min;
  x3 = max;
  if ( vnl_math_abs( max - median ) > vnl_math_abs( min - median ) ) {
    x1 = median;
    x2 = median + C * ( max - median );
  }
  else {
    x2 = median;
    x1 = median - C * ( median - min );
  }

  f1 = -kernel_density( res_begin, res_end, x1, h );
  f2 = -kernel_density( res_begin, res_end, x2, h );
  while ( vnl_math_abs( x3 - x0 ) > tol * vnl_math_abs( x1 ) + vnl_math_abs( x2 ) ) {
    if ( f2 < f1 ) {
      shft3( x0, x1, x2, R * x2 + C * x3 );
      shft2( f1, f2, -kernel_density( res_begin, res_end, x2, h ) );
    }
    else {
      shft3( x3, x2, x1, R * x1 + C * x0 );
      shft2( f2, f1, -kernel_density( res_begin, res_end, x1, h ) );
    }
  }
  if (f1 < f2)
    return x1;
  else
    return x2;
}
//  {
//    unsigned int n = res_end - res_begin;
//    double h = bandwidth(res_begin, res_end, prior_scale); 
//    assert(h!=0);
//    double f = 0;

//    vect_const_iter begin = res_begin;
//    const unsigned int IMAX = 100;
//    const double eps = 1.0e-09;
//    unsigned int count = 0;

//    vect_const_iter mloc = vcl_min_element(res_begin, res_end);
//    double min = *mloc - h;
//    mloc = vcl_max_element(res_begin, res_end);
//    double max = *mloc + h;
//    vcl_vector<double> res(res_begin, res_end);
//    vcl_vector<double>::iterator loc = res.begin() + n / 2;
//    vcl_nth_element(res.begin(), loc, res.end());
//    double median = *loc;

//    //initial guess
//    double x = median;
//    double dx = 0;

//    double rtn;
//    while ( count < IMAX ) {
//      ++count;
//      double f = 0;
//      double f_prime = 0;
//      double f_double_prime = 0;
//      for ( begin=res_begin; begin != res_end; ++begin ) {
//        f += kernel_function ( ( *begin - x ) / h );
//        f_prime += kernel_prime ( ( *begin - x ) / h , h );
//        f_double_prime += kernel_double_prime ( ( *begin - x ) / h , h );
//      }

//      if (f_double_prime == 0) {
//        vcl_cerr << "f_double_prime=0, x=" << x << ", min=" << min << ", max=" << max << vcl_endl;
//      }
//      if (f > max_f) {
//        max_f = f;
//        rtn = x;
//      }
//      assert(f_double_prime!=0);
//      //maximize f
//      dx = f_prime / vnl_math_abs(f_double_prime);
//      if (vnl_math_abs(dx) < eps) return x;
//      x += dx;

//      // Out of the range [min, max], f, f_prime, and f_double_prime are zero.
//      if (x > max)  { vcl_cerr << x << " > " << max << vcl_endl; x = 0.5*(min+median); }
//      if (x < min)  { vcl_cerr << x << " < " << min << vcl_endl; x = 0.5*(max+median); }
//    }
//    vcl_cerr << "Warning : rrel_kernel_density_obj : maximum number of iterations exceeded. " << vcl_endl;
//    return x;
//  }

double 
rrel_kernel_density_obj::bandwidth(vect_const_iter res_begin, vect_const_iter res_end,
         double prior_scale) const
{
  unsigned int n = res_end - res_begin;
  double scale;

  switch ( scale_type_ ) {
  case RREL_KERNEL_MAD: {
    //A median absolute deviations (MAD) scale estimate.

    //Here I avoid using rrel_util_median_abs_dev_scale 
    //because it assumes residuals are zero-based and have a Gaussian distribution.

    vcl_vector<double> residuals(res_begin, res_end);
    vcl_vector<double>::iterator loc = residuals.begin() + n / 2;
    vcl_nth_element( residuals.begin(), loc, residuals.end() );
    
    double res_median = *loc;
    vcl_vector<double> abs_res_median;
    abs_res_median.reserve( n );
    for (unsigned int i=0; i<n; i++ ) {
      abs_res_median.push_back( vnl_math_abs( residuals[i] - res_median ) );
    }
    loc = abs_res_median.begin() + n / 2;
    vcl_nth_element( abs_res_median.begin(), loc, abs_res_median.end() );
    // c=0.5 is chosen to avoid over-smoothing of the estimated density.
    scale = 0.5 * (*loc);
    break;
  }

  case RREL_KERNEL_PRIOR: {
    scale = prior_scale;
    break;
  }

  case RREL_KERNEL_MUSE: {
    rrel_muset_obj muse(n);
    scale = muse.fcn( res_begin, res_end );
    break;
  }
  }

  // h = [243 * R(K) / 35 / Mu(K)^2 / n]^0.2 * scale
  // R(K) = Integral ( K(u)^2 ) du
  // Mu(K) = Integral ( u^2 * K(u) ) du
  const double c = 75087.0 / 143;
  return vcl_pow( c / n , 0.2 ) * scale;
}

double
rrel_kernel_density_obj::kernel_density(vect_const_iter res_begin, vect_const_iter res_end,
         double x, double h) const
{
  double f=0;
  unsigned int n = res_end - res_begin;
  for ( ; res_begin!= res_end; ++res_begin ) {
    f += kernel_function( ( *res_begin - x ) / h );
  }
  f /= n*h;
  return f;
}

double 
rrel_kernel_density_obj::kernel_function(double u) const
{
  if (vnl_math_abs(u) > 1)
    return 0;

  double t = 1 - u * u;
  const double c = 1.09375;
  return c * t * t * t;
}

double
rrel_kernel_density_obj::kernel_prime(double u, double h) const
{
  if (vnl_math_abs(u) > 1)
    return 0;
  double d = 1 - u*u;
  return 6.5625 / h * u * d * d;
}

double
rrel_kernel_density_obj::kernel_double_prime(double u, double h) const
{
  if (vnl_math_abs(u) > 1)
    return 0;
  double d = u*u;
  return -6.5625 / (h * h) * (1 - d) * (1 - 5 * d);
}


