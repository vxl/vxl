// This is rpl/rrel/rrel_muse_table.cxx
#include "rrel_muse_table.h"
#include <rrel/rrel_misc.h>
#include <vnl/vnl_math.h>

#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

bool operator< ( rrel_muse_key_type const& left_t, rrel_muse_key_type const& right_t )
{
  return left_t.n_ < right_t.n_
    || ( left_t.n_ == right_t.n_ && left_t.k_ < right_t.k_ );
}

double
rrel_muse_table::expected_kth( unsigned int k, unsigned int n )
{
  assert( 0<k && k<= n );
  rrel_muse_key_type key(k,n);
  rrel_muse_table_entry& entry = table_[key];
  if ( ! entry . initialized_ )
    calculate_all( k, n, entry );
  return entry . expected_;
}

double
rrel_muse_table::standard_dev_kth( unsigned int k, unsigned int n )
{
  assert( 0<k && k<=n );
  rrel_muse_key_type key(k,n);
  rrel_muse_table_entry& entry = table_[key];
  if ( ! entry . initialized_ )
    calculate_all( k, n, entry );
  return entry . standard_dev_;
}

double
rrel_muse_table::muset_divisor( unsigned int k, unsigned int n )
{
  assert( 0<k && k<= n );
  rrel_muse_key_type key(k,n);
  rrel_muse_table_entry& entry = table_[key];
  if ( ! entry . initialized_ )
    calculate_all( k, n, entry );
  return entry . muse_t_divisor_;
}


double
rrel_muse_table::muset_sq_divisor( unsigned int k, unsigned int n )
{
  assert( 0<k && k<= n );
  rrel_muse_key_type key(k,n);
  rrel_muse_table_entry& entry = table_[key];
  if ( ! entry . initialized_ )
    calculate_all( k, n, entry );
  return entry . muse_t_sq_divisor_;
}

void
rrel_muse_table::calculate_all( unsigned int k, unsigned int n,
                                rrel_muse_table_entry & entry )
{
  entry . initialized_ = true;
  entry . expected_ = calculate_expected( k, n );
  entry . standard_dev_ = calculate_standard_dev( k, n, entry . expected_ );
  entry . muse_t_divisor_ = calculate_divisor( k, n, entry . expected_ );
  entry . muse_t_sq_divisor_ = calculate_sq_divisor( k, n, entry . expected_ );
}

double
rrel_muse_table::calculate_expected( unsigned int k, unsigned int n ) const
{
  return rrel_misc_gaussian_cdf_inv(0.5*(1.0+((double)k / (double)(n+1))));
}


double
rrel_muse_table::calculate_standard_dev( unsigned int k, unsigned int n,
                                         double expected_kth ) const
{
  double pk, qk, Qk, pQk, Qk_prime, Qk_dprime, Qk_tprime, vrk;

  pk = (double) k / (double) (n+1); // might want alpha beta correction
  qk = 1.0 - pk;

  // calculate_ the inverse cdf (might want to an alpha-beta correction)
  Qk = expected_kth;  // ak(k, N);   // inverse cdf of absolute residuals

  // density of absolute residual evaluated at Qk
  pQk = vcl_exp( -0.5 * Qk*Qk) * vcl_sqrt(2.0 / vnl_math::pi);

  // first derivative of Qk
  Qk_prime = 1.0/pQk;

  /*
  //  Low order approximation
  vrk = (pk*qk/(double)(n+2)) * Qk_prime*Qk_prime;
  */

  // second derivative of Qk
  Qk_dprime = Qk/(pQk*pQk);

  // third derivative of Qk
  Qk_tprime = ( 1.0 + 2.0 * Qk*Qk ) / (pQk*pQk*pQk);

  //  Higher order approximation
  vrk = (pk*qk/(double)(n+2)) * Qk_prime*Qk_prime
        + (pk*qk/((double)((n+2)*(n+2)))) * ( 2.0*(qk - pk)*Qk_prime*Qk_dprime
        + pk*qk*(Qk_prime*Qk_tprime + 0.5*Qk_dprime*Qk_dprime));

  return vcl_sqrt(vrk);
}


double
rrel_muse_table::calculate_divisor( unsigned int /* k */, unsigned int n,
                                    double expected_kth ) const
{
  return (n+1)*vcl_sqrt(2/vnl_math::pi)*(1.0-vcl_exp(-vnl_math_sqr(expected_kth)/2.0));
}

double
rrel_muse_table::calculate_sq_divisor( unsigned int k, unsigned int n,
                                       double expected_kth ) const
{
  return k - (n+1) * expected_kth * vcl_sqrt(2/vnl_math::pi)
    * vcl_exp(-vnl_math_sqr(expected_kth)/2.0);
}

