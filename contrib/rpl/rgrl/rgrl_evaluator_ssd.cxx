#include <vcl_iostream.h>
#include <rgrl/rgrl_evaluator_ssd.h>
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>

// Normalized SSD
double
rgrl_evaluator_ssd::
evaluate( vcl_vector< double > const& a, 
          vcl_vector< double > const& b, 
          vcl_vector< double > const& weight ) const
{
  assert( a.size() == b.size() && a.size() > 2);

  double sum_a = 0, sum_b = 0;
  for( unsigned i = 0; i < a.size(); ++i ) {
    sum_a += a[ i ];
    sum_b += b[ i ];
  }
  double mean_a = sum_a / a.size();
  double mean_b = sum_b / b.size();
  double sigma_a = 0, sigma_b = 0;

  for( unsigned i = 0; i < a.size(); ++i ) {
    sigma_a += vnl_math_sqr( a[ i ] - mean_a );
    sigma_b += vnl_math_sqr( b[ i ] - mean_b );
  }

  sigma_a = vcl_sqrt( sigma_a / (a.size()-1) );
  sigma_b = vcl_sqrt( sigma_b / (b.size()-1) );

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
    
    double ff = (double) vnl_math_sqr ( aa - bb ) / a.size();
    f += weight[i] * vcl_sqrt ( ff );
  }

  return f;
}
