// This is brl/bbas/bsta/algo/bsta_parzen_updater.h
#ifndef bsta_parzen_updater_h_
#define bsta_parzen_updater_h_
//:
// \file
// \brief Parzen updaters
// \author Joseph L. Mundy
// \date October 13, 2008
//
// This file contains updaters based on
// Parzen distributions
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <bsta/bsta_parzen_sphere.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: A parzen window (kernel) updater.
// If a new sample is within tol of the existing samples then don't insert.
// Otherwise if max number of samples is reached, replace the nearest sample.
template <class parzen_dist_>
class bsta_parzen_updater
{
 public:
  typedef typename parzen_dist_::math_type T;
  typedef typename parzen_dist_::vector_type vector_;
  enum { data_dimension = parzen_dist_::dimension };

  //: for compatibility with vpdl/vpdt
  typedef typename parzen_dist_::field_type field_type;

  // Constructor
  bsta_parzen_updater(T tol, unsigned max_samples)
  : tol_(tol), max_samples_(max_samples) {}

  //: The update functor
  void operator() (parzen_dist_& pdist, const vector_& sample) const;
 private:
  T tol_;
  unsigned max_samples_;
};

//: A parzen window (kernel) updater.
// If a new sample is within tol of the existing samples then don't insert.
// Otherwise if maximum number of samples has been reached, then compute a new
// bandwidth estimate based on the most probable fraction of the existing
// samples.
template <class parzen_dist_>
class bsta_parzen_adapt_bw_updater
{
 public:
  typedef typename parzen_dist_::math_type T;
  typedef typename parzen_dist_::vector_type vector_;
  enum { data_dimension = parzen_dist_::dimension };

  //: for compatibility with vpdl/vpdt
  typedef typename parzen_dist_::field_type field_type;
  typedef parzen_dist_ distribution_type;


  // Constructor
  bsta_parzen_adapt_bw_updater(T tol, unsigned max_samples,
                               T frac_backgnd = T(1))
  : tol_(tol), max_samples_(max_samples),
    frac_background_(frac_backgnd) {}

  //: The update functor
  void operator() (parzen_dist_& pdist, const vector_& sample) const;
 private:
  T tol_;
  unsigned max_samples_;
  T frac_background_;
};

#endif // bsta_parzen_updater_h_
