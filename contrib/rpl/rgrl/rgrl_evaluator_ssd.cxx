#include "rgrl_evaluator_ssd.h"
//:
// \file
#include <vnl/vnl_math.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Normalized SSD
double
rgrl_evaluator_ssd::
evaluate( std::vector< double > const& a,
          std::vector< double > const& b,
          std::vector< double > const& weight ) const
{
  assert( a.size() == b.size() && a.size() > 2);

  double sum_a = 0, sum_b = 0;
  for ( unsigned i = 0; i < a.size(); ++i ) {
    sum_a += a[ i ];
    sum_b += b[ i ];
  }
  double mean_a = sum_a / a.size();
  double mean_b = sum_b / b.size();
  double sigma_a = 0, sigma_b = 0;

  for ( unsigned i = 0; i < a.size(); ++i ) {
    sigma_a += vnl_math::sqr( a[ i ] - mean_a );
    sigma_b += vnl_math::sqr( b[ i ] - mean_b );
  }

  sigma_a = std::sqrt( sigma_a / (a.size()-1) );
  sigma_b = std::sqrt( sigma_b / (b.size()-1) );

  double f = 0;
  double aa, bb;
  for ( unsigned i = 0 ; i < a.size(); ++i ) {

    // The region is constant
    if ( sigma_a == 0 )
      aa = 0;
    else
      aa = ( a[ i ] - mean_a ) / sigma_a;

    if ( sigma_b == 0 )
      bb = 0;
    else
      bb = ( b[ i ] - mean_b ) / sigma_b;

    double ff = (double) vnl_math::sqr ( aa - bb ) / a.size();
    f += weight[i] * std::sqrt ( ff );
  }

  return f;
}
