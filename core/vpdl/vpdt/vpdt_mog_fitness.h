// This is core/vpdl/vpdt/vpdt_mog_fitness.h
#ifndef vpdt_mog_fitness_h_
#define vpdt_mog_fitness_h_
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 8, 2009
// \brief Ordering functions for sorting mixtures of Gaussian components

#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_gaussian.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: The Stauffer-Grimson ordering function of mixture component fitness
//  This is a generalization to non-scalar covariance
template <class gaussian_type, class Disambiguate = void>
struct vpdt_mog_fitness
{
  typedef typename gaussian_type::metric_type Metric;
  typedef typename vpdt_dist_traits<gaussian_type>::scalar_type T;

  static bool order (const gaussian_type& g1, const T& w1,
                     const gaussian_type& g2, const T& w2)
  {
    const unsigned int d = g1.dimension();
    assert(d == g2.dimension());
    T w1_2 = w1*w1, w2_2 = w2*w2;
    T v1 = w1_2, v2 = w2_2;
    for (unsigned int i=1; i<d; ++i){
      v1 *= w1_2;
      v2 *= w2_2;
    }
    return v1/Metric::covar_det(g1.mean,g1.covar) >
    v2/Metric::covar_det(g2.mean,g2.covar);
  }
};


//: helper class to check that two types are equal
template <class T1, class T2>
struct vpdt_is_equal;
//: helper class to check that two types are equal
template <class T>
struct vpdt_is_equal<T,T> { typedef void type; };


//: The Stauffer-Grimson ordering function of mixture component fitness
//  This is the simplified (original) version that applies to scalar covariance
template <class gaussian_type>
struct vpdt_mog_fitness<gaussian_type,
                        typename vpdt_is_equal<typename vpdt_dist_traits<gaussian_type>::scalar_type,
                                               typename gaussian_type::covar_type>::type >
{
  typedef typename vpdt_dist_traits<gaussian_type>::scalar_type T;

  static bool order (const gaussian_type& g1, const T& w1,
                     const gaussian_type& g2, const T& w2)
  {
    return w1*w1/g1.covar > w2*w2/g2.covar;
  }
};


#endif // vpdt_mog_fitness_h_
