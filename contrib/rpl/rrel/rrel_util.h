// This is rpl/rrel/rrel_util.h
#ifndef rrel_util_h_
#define rrel_util_h_
//:
//  \file
//  \author Chuck Stewart
//  \date   Summer 2001

#include <vcl_iterator.h>

//: \brief Compute the median absolute deviation (MAD) scale estimate of the residuals.
//
// Find the median absolve deviation (MAD) scale estimate of a set of
// residuals.  Assume a Gaussian target distribution.  The input
// residuals may be signed or unsigned values, but they are converted
// to unsigned.  The order of the residuals is destroyed.
//
// The iterators should be random access iterators.
//
// The dummy parameter is used to determine the return type:
// \code
//    float s = rrel_util_median_abs_dev_scale( v.begin(), v.end(), 1, (float*)0 );
// \endcode
//
template <class O, class RanIter>
O
rrel_util_median_abs_dev_scale( const RanIter& begin,  const RanIter& end, int dof, O* );

//: Convenience function.
// Calls the other rrel_util_median_abs_dev_scale() with a return type
// of double.
//
template <class T>
double
rrel_util_median_abs_dev_scale( const T& begin,  const T& end, int dof=1 )
{
  return rrel_util_median_abs_dev_scale( begin,  end, dof, (double*)0 );
}

//: \brief Compute the scale using robustly weighted residuals.
//
//  Compute the scale using robustly weighted residuals. The dummy
//  parameter is used to determine the return type. Internal
//  calculations are also done with this type.
//
//  The iterators should be input iterators.
//
// \code
//    float s = rrel_util_weighted_scale( r.begin(), r.end(),
//                                        w.begin(), w.end(), 1, (float*)0 );
// \endcode
//
template <class O, class InpIter>
O
rrel_util_weighted_scale( const InpIter& residuals_first, const InpIter& residuals_end,
                          const InpIter& weights_first, int dof, O* );

#if !VCL_TEMPLATE_MATCHES_TOO_OFTEN // not for compilers with overload problems

//: Convenience function.
// Calls the other rrel_util_weighted_scale() with a return type of
// double.
//
template <class InpIter>
inline double
rrel_util_weighted_scale( const InpIter& residuals_first, const InpIter& residuals_end,
                          const InpIter& weights_first, int dof=1 )
{
  return rrel_util_weighted_scale( residuals_first, residuals_end,
                                   weights_first, dof, (double*)0 );
}

#endif

//: Compute the median and the scale (relative to the median).
//
//  Find the median and then, using the median of absolute
//  deviations from the median, the scale.  Assume a Gaussian target
//  distribution.  Both the order and the values themselves are changed.
//  Random access iterators are required.
//
template <class T, class RanIter>
void rrel_util_median_and_scale( RanIter first, RanIter last,
                                 T& median, T& scale,
                                 int dof=1 );


//: Compute the median and the scale (relative to the median).
//
//  Same as rrel_util_median_and_scale(), except that the input is
//  copied first, and so is unchanged.
//
template <class T, class InpIter>
void rrel_util_median_and_scale_copy( InpIter first, InpIter last,
                                      T& median, T& scale,
                                      int dof=1 );


//: \brief Compute the center and half width of the narrowest interval containing half the points in the residuals.
//
//  Find the center and half width of the narrowest interval
//  containing half the points in the input set of residuals, accessed
//  through the iterators first and last.  This is the "intercept
//  adjustment" technique of Rousseeuw (J. Amer. Stat. Assoc. 1984).
//  The order of the residuals is destroyed and in fact, as a side
//  effect, the residuals are sorted.  The residuals are assumed to be
//  signed values.
//
template <class T, class RanIter>
void rrel_util_intercept_adjustment( RanIter first, RanIter last,
                                     T & center, T & half_width,
                                     int dof=1 );


//: \brief Compute the center and half width of the narrowest interval containing half the points in the residuals.
//
//  Same as rrel_util_intercept_adjustment(), except that the input is
//  copied first, and so is unchanged.
//
template <class T, class InpIter>
void rrel_util_intercept_adjustment_copy( InpIter first, InpIter last,
                                          T & center, T & half_width,
                                          int dof=1 );


//: \brief Use the intercept adjustment technique to estimate the robust mean, standard deviation, and inlier fraction.
//
template <class T, class RanIter>
void rrel_util_intercept_adjust_stats( RanIter first, RanIter last,
                                       T & robust_mean, T & robust_std, T & inlier_frac,
                                       int dof=1 );


//: \brief Use the intercept adjustment technique to estimate the robust mean, standard deviation, and inlier fraction.
//
//  Same as rrel_util_intercept_adjustment_stats(), except that the input is
//  copied first, and so is unchanged.
//
template <class T, class InpIter>
void rrel_util_intercept_adjust_stats_copy( InpIter first, InpIter last,
                                            T & robust_mean, T & robust_std, T & inlier_frac,
                                            int dof=1 );

#ifdef VCL_GCC_32
# include "rrel_util.txx"
#endif

#endif // rrel_util_h_
