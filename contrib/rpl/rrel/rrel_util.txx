// This is rpl/rrel/rrel_util.txx
#ifndef rrel_util_txx_
#define rrel_util_txx_

#include "rrel_util.h"

#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_vector.h>

#include <vnl/vnl_math.h>


template <class O, class T>
O
rrel_util_median_abs_dev_scale( const T& begin,  const T& end, int dof, O* /*dummy*/ )
{
  long count = long(end - begin); // VC6 & 7 has broken iterator_traits
  assert( count > 0);

  if ( count <= dof )
    return 0;

  for ( T i=begin; i!=end; ++i ) {
    *i = vnl_math_abs( *i );
  }
  T loc = begin + ((count-dof)/2 + dof);
  vcl_nth_element( begin, loc, end );
  return O (1.4826 * (1 + 5.0/(count-dof)) * *loc);
}


template <class O, class T>
O
rrel_util_weighted_scale( const T& residuals_first, const T& residuals_end,
                          const T& weights_first, int dof, O* /*dummy*/ )
{
  O sum = 0;
  O sum_weights = 0;
  int num = 0;

  T r_itr = residuals_first;
  T w_itr = weights_first;
  for ( ; r_itr != residuals_end; ++ r_itr, ++ w_itr ) {
    sum += *w_itr * vnl_math_sqr( *r_itr );
    sum_weights += *w_itr;
    ++num;
  }
  if ( num <= dof )
    return 0;

  O divisor = sum_weights * ( num - dof ) / num;
  return vcl_sqrt( sum / divisor );
}


template <class T, class Ran>
void rrel_util_median_and_scale( Ran first, Ran last,
                                 T& median, T& scale,
                                 int dof )
{
  long count = long(last-first); // VC6 & 7 has broken iterator_traits
  assert( count > 0 );
  assert( count > dof );

  Ran loc = first + count/2;
  vcl_nth_element( first, loc, last );
  median = *loc;
  for ( Ran i=first; i!=last; ++i ) {
    *i = vnl_math_abs(*i-median);
  }
  ++loc;
  vcl_nth_element( first, loc, last );
  scale = T(1.4826 * (1 + 5.0/(count-dof)) * *loc);
}


template <class T, class InpIter>
void rrel_util_median_and_scale_copy( InpIter first, InpIter last,
                                      T& median, T& scale,
                                      int dof )
{
  // FIXME: scratch should be vcl_vector<
  // vcl_iterator_traits<InpIter>::value_type >, but this is not
  // supported under all compilers. In particular, VC++ doesn't
  // support it for vector iterators.
  //
  vcl_vector<T> scratch;
  for ( ; first != last; ++first )
    scratch.push_back( *first );
  rrel_util_median_and_scale( scratch.begin(), scratch.end(), median, scale, dof );
}


template <class T, class Ran>
void rrel_util_intercept_adjustment( Ran first, Ran last,
                                     T & center, T & half_width,
                                     int dof )
{
  long count = long(last-first); // VC6 & 7 has broken iterator_traits
  assert( count > dof );
  vcl_sort( first, last );
  int num_in_interval = (count-dof)/2 + dof;
  if ( num_in_interval > count ) num_in_interval = count;
  T min_start = *first;
  T min_width = *(first+num_in_interval-1) - min_start;
  Ran start=first+1;
  Ran end=first+num_in_interval;
  for ( ; end != last; ++start, ++end ) {
    T width = *end - *start;
    if ( width < min_width ) {
      min_start = *start;
      min_width = width;
    }
  }
  half_width = min_width/2;
  center = min_start + half_width;
}


template <class T, class InpIter>
void rrel_util_intercept_adjustment_copy( InpIter first, InpIter last,
                                          T & center, T & half_width,
                                          int dof )
{
  // FIXME: scratch should be vcl_vector<
  // vcl_iterator_traits<InpIter>::value_type >, but this is not
  // supported under all compilers. In particular, VC++ doesn't
  // support it for vector iterators.
  //
  vcl_vector<T> scratch;
  for ( ; first != last; ++first )
    scratch.push_back( *first );
  rrel_util_intercept_adjustment( scratch.begin(), scratch.end(),
                                  center, half_width, dof );
}


template <class T, class Ran>
void rrel_util_intercept_adjust_stats( Ran first, Ran last,
                                       T & robust_mean, T & robust_std, T & inlier_frac,
                                       int dof )
{
  long count = long(last-first); // VC6 & 7 has broken iterator_traits
  assert( count >= dof );
  T center, half_width;
  rrel_util_intercept_adjustment( first, last, center, half_width, dof );

  T std_dev = half_width * T(1.4826) * T( 1 + 5.0/(count-dof) );
  const T mu = 2.5;
  T bound = mu * std_dev;

  Ran begin_itr=first-1;
  while ( *(++begin_itr) < center-bound ) ;
  Ran end_itr=begin_itr;
  T sum = *begin_itr;
  while ( ++end_itr < last && *end_itr <= center+bound ) {
    sum += *end_itr;
  }
  long inliers = long(end_itr - begin_itr); // VC6 & 7 has broken iterator_traits
  robust_mean = sum / inliers;
  inlier_frac = T(inliers) / T(count);

  T sum_sq=0;
  for ( Ran i=begin_itr; i!=end_itr; ++i ) {
    sum_sq += vnl_math_sqr( *i - robust_mean );
  }
  robust_std = T( vcl_sqrt(sum_sq / (inliers-dof)) );
}


template <class T, class InpIter>
void rrel_util_intercept_adjust_stats_copy( InpIter first, InpIter last,
                                            T & robust_mean, T & robust_std, T & inlier_frac,
                                            int dof )
{
  // FIXME: scratch should be vcl_vector<
  // vcl_iterator_traits<InpIter>::value_type >, but this is not
  // supported under all compilers. In particular, VC++ doesn't
  // support it for vector iterators.
  //
  vcl_vector<T> scratch;
  for ( ; first != last; ++first )
    scratch.push_back( *first );
  rrel_util_intercept_adjust_stats( scratch.begin(), scratch.end(),
                                    robust_mean, robust_std, inlier_frac, dof );
}


// Instantiation macros
//
#undef RREL_UTIL_INSTANTIATE_RAN_ITER
#define RREL_UTIL_INSTANTIATE_RAN_ITER(VALUE_T, RAN_ITER) \
template VALUE_T \
rrel_util_median_abs_dev_scale( const RAN_ITER&,  const RAN_ITER&, int dof, VALUE_T* ); \
template double \
rrel_util_median_abs_dev_scale( const RAN_ITER&,  const RAN_ITER&, int dof ); \
template VALUE_T \
rrel_util_weighted_scale( const RAN_ITER& residuals_first, const RAN_ITER& residuals_end, \
                          const RAN_ITER& weights_first, int dof, VALUE_T* ); \
template \
void rrel_util_median_and_scale( RAN_ITER first, RAN_ITER last, \
                                 VALUE_T& median, VALUE_T& scale, \
                                 int dof ); \
template \
void rrel_util_median_and_scale_copy( RAN_ITER first, RAN_ITER last, \
                                      VALUE_T& median, VALUE_T& scale, \
                                      int dof ); \
template \
void rrel_util_intercept_adjustment( RAN_ITER first, RAN_ITER last, \
                                     VALUE_T & center, VALUE_T & half_width, \
                                     int dof ); \
template \
void rrel_util_intercept_adjustment_copy( RAN_ITER first, RAN_ITER last, \
                                          VALUE_T & center, VALUE_T & half_width, \
                                          int dof ); \
template \
void rrel_util_intercept_adjust_stats( RAN_ITER first, RAN_ITER last, \
                                       VALUE_T & robust_mean, VALUE_T & robust_std, VALUE_T & inlier_frac, \
                                       int dof ); \
template \
void rrel_util_intercept_adjust_stats_copy( RAN_ITER first, RAN_ITER last, \
                                            VALUE_T & robust_mean, VALUE_T & robust_std, VALUE_T & inlier_frac, \
                                            int dof )

#undef RREL_UTIL_INSTANTIATE_INP_ITER
#define RREL_UTIL_INSTANTIATE_INP_ITER(VALUE_T, INP_ITER) \
template VALUE_T \
rrel_util_weighted_scale( const INP_ITER& residuals_first, const INP_ITER& residuals_end, \
                          const INP_ITER& weights_first, int dof, VALUE_T* ); \
template \
void rrel_util_median_and_scale_copy( INP_ITER first, INP_ITER last, \
                                      VALUE_T& median, VALUE_T& scale, \
                                      int dof ); \
template \
void rrel_util_intercept_adjustment_copy( INP_ITER first, INP_ITER last, \
                                          VALUE_T & center, VALUE_T & half_width, \
                                          int dof ); \
template \
void rrel_util_intercept_adjust_stats_copy( INP_ITER first, INP_ITER last, \
                                            VALUE_T & robust_mean, VALUE_T & robust_std, VALUE_T & inlier_frac, \
                                            int dof )

#endif // rrel_util_txx_
