// This is rpl/rrel/rrel_mlesac_obj.cxx
#include "rrel_mlesac_obj.h"

#include <rrel/rrel_util.h>
#include <vnl/vnl_math.h>

#include <vcl_cstdlib.h>
#include <vcl_cmath.h>


namespace {
  inline double sqr( double x ) { return x*x; }
}


rrel_mlesac_obj::rrel_mlesac_obj(unsigned int residual_dof, double outlier_sigma, double outlier_frac)
  : outlier_sigma_(outlier_sigma),
    outlier_frac_(outlier_frac),
    residual_dof_(residual_dof)
{
}

double
rrel_mlesac_obj::fcn( vect_const_iter begin, vect_const_iter end,
                      vect_const_iter scale,
                      vnl_vector<double>* /* param_vector */ ) const
{
  double value=0;
  double pi,p0,zi;
  vect_const_iter begin0 = begin;
  unsigned long num_residual = end - begin;
  double mult1 = 1.0 / (vcl_sqrt( 2 * vnl_math::pi ));

  double inlier_frac = 1.0;
  double new_inlier_frac = 1 - outlier_frac_;

  const double EPS = 0.01;

  //EM algorithm to get outlier_frac, the mixing parameter
  while ( new_inlier_frac > EPS && vcl_abs((new_inlier_frac - inlier_frac) / inlier_frac) > EPS) {
    begin = begin0;
    inlier_frac = new_inlier_frac;
    new_inlier_frac = 0;
    for  (; begin != end; ++begin, ++scale) {
      double const1 = vcl_pow(mult1 / (*scale), (int)residual_dof_) ;
      pi = inlier_frac * const1 * vcl_exp( - sqr(*begin) / ( 2.0 * sqr(*scale) ) );
      p0 = (1 - inlier_frac) / outlier_sigma_;
      zi = pi / ( pi + p0 );
      new_inlier_frac += zi;
    }
    new_inlier_frac = new_inlier_frac / num_residual;
  }

  begin = begin0;
  //the negative log likelihood
  for ( ; begin != end; ++begin) {
    double const1 = vcl_pow(mult1 / (*scale), (int)residual_dof_) ;
    pi = new_inlier_frac * const1 * vcl_exp( - sqr(*begin) / ( 2.0 * sqr(*scale) ) );
    p0= ( 1 - new_inlier_frac ) / outlier_sigma_;
    value -= vcl_log( pi + p0 );
  }

  return value;
}

double
rrel_mlesac_obj::fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* ) const
{
  double value=0;
  double pi,p0,zi;
  vect_const_iter begin0 = begin;
  unsigned long num_residual = end - begin;

  double inlier_frac = 1.0;
  double new_inlier_frac = 1 - outlier_frac_;
  double mult1 = 1.0 / (vcl_sqrt( 2 * vnl_math::pi ) * scale);
  double const1 = vcl_pow(mult1, (int)residual_dof_) ;
  double exp_mult2 = -1.0 / (2.0 * sqr(scale));

  const double EPS = 0.01;

  //EM algorithm to get outlier_frac, the mixing parameter
  while ( new_inlier_frac > EPS && vcl_abs((new_inlier_frac - inlier_frac) / inlier_frac) > EPS) {
    begin = begin0;
    inlier_frac = new_inlier_frac;
    new_inlier_frac = 0;
    for  (; begin != end; ++begin) {
      pi = inlier_frac * const1 * vcl_exp( sqr(*begin) * exp_mult2 );
      p0 = (1 - inlier_frac) / outlier_sigma_;
      zi = pi / ( pi + p0 );
      new_inlier_frac += zi;
    }
    new_inlier_frac = new_inlier_frac / num_residual;
  }

  begin = begin0;
  //the negative log likelihood
  for ( ; begin != end; ++begin) {
    pi = new_inlier_frac * const1 * vcl_exp( sqr(*begin) * exp_mult2 );
    p0= ( 1 - new_inlier_frac ) / outlier_sigma_;
    value -= vcl_log( pi + p0 );
  }

  return value;
}
