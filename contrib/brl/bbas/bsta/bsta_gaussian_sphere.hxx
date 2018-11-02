// This is brl/bbas/bsta/bsta_gaussian_sphere.hxx
#ifndef bsta_gaussian_sphere_hxx_
#define bsta_gaussian_sphere_hxx_
//:
// \file

#include <iostream>
#include <limits>
#include "bsta_gaussian_sphere.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_erf.h>

namespace
{
  //: Unrol the Mahalanobis distance calculation
  template <class T, unsigned n, unsigned index>
  struct bsta_gaussian_sphere_compute_dot
  {
    static inline T value(const vnl_vector_fixed<T,n>& d)
    {
      return d[index-1]*d[index-1]
           + bsta_gaussian_sphere_compute_dot<T,n,index-1>::value(d);
    }
  };

  //: base case
  // this is partial specialization
  template <class T, unsigned n>
  struct bsta_gaussian_sphere_compute_dot<T,n,0>
  {
    static inline T value(const vnl_vector_fixed<T,n>& /*d*/)
    { return 0; }
  };

  //: base case
  // this is partial specialization
  template <class T>
      struct bsta_gaussian_sphere_compute_dot<T,1,1>
  {
    static inline T value(const T& d)
    { return d*d; }
  };


  //: Unroll the determinant calculation
  template <class T, unsigned n, unsigned index>
  struct bsta_gaussian_sphere_determinant
  {
    static inline T value(const T& var)
    {
      return var * bsta_gaussian_sphere_determinant<T,n,index-1>::value(var);
    }
  };

  //: base case
  // this is partial specialization
  template <class T, unsigned n>
  struct bsta_gaussian_sphere_determinant<T,n,0>
  {
    static inline T value(const T& /*var*/) { return 1; }
  };
} // namespace


//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
T
bsta_gaussian_sphere<T,n>::sqr_mahalanobis_dist(const vector_& pt) const
{
  if (var_<=T(0))
    return std::numeric_limits<T>::infinity();
  vector_ d = bsta_gaussian<T,n>::mean_-pt;
  return bsta_gaussian_sphere_compute_dot<T,n,n>::value(d)/var_;
}

//: Unrol the compute probability calculation
//  The general induction step
template <class T, class vector_, unsigned n, unsigned index>
struct bsta_gaussian_sphere_compute_probability_box
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var
                       )
  {
    if (var<=T(0))
      return std::numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/std::sqrt(sigma_sq_2);
    double temp = vnl_erf(max_minus_mean[index]*s2);
    temp -= vnl_erf(min_minus_mean[index]*s2);
    T res = static_cast<T>(0.5*temp);
    res *= bsta_gaussian_sphere_compute_probability_box<T,vector_,n,index-1>::value(min_minus_mean,
                                                                                    max_minus_mean,
                                                                                    var);
    return res;
  }
};

//: base case
//  This is partial specialization
template <class T, class vector_, unsigned n>
struct bsta_gaussian_sphere_compute_probability_box<T,vector_,n,0>
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var)
  {
    if (var<=T(0))
      return std::numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/std::sqrt(sigma_sq_2);
    double temp = vnl_erf(max_minus_mean[0]*s2);
    temp -= vnl_erf(min_minus_mean[0]*s2);
    return static_cast<T>(0.5*temp);
  }
};


//: base case
//  This is partial specialization
template <class T, class vector_>
struct bsta_gaussian_sphere_compute_probability_box<T,vector_,1,0>
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var)
  {
    if (var<=T(0))
      return std::numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/std::sqrt(sigma_sq_2);
    double temp = vnl_erf(max_minus_mean*s2);
    temp -= vnl_erf(min_minus_mean*s2);
    return static_cast<T>(0.5*temp);
  };
};

//: The probability that a sample lies inside a n-d bounding box
//  \note min_pt and max_pt are the corners of the box
template <class T, unsigned int n>
T bsta_gaussian_sphere<T,n>::probability(const vector_& min_pt,
                                         const vector_& max_pt) const
{
  vector_ min_minus_mean = min_pt-bsta_gaussian<T,n>::mean_;
  vector_ max_minus_mean = max_pt-bsta_gaussian<T,n>::mean_;
  return bsta_gaussian_sphere_compute_probability_box<T, vector_, n, n-1>::value(min_minus_mean,
                                                                                 max_minus_mean,
                                                                                 var_);
}

//: The determinant of the covariance matrix
template <class T, unsigned int n>
void
bsta_gaussian_sphere<T,n>::compute_det()
{
  det_covar_ = bsta_gaussian_sphere_determinant<T,n,n>::value(var_);
  assert(det_covar_ >= 0);
}


//: Unrol the sampling calculation
//  The general induction step
template <class T, class vector_, unsigned n, unsigned index>
struct var_from_dist
{
  static inline vector_ value(const T& var, vnl_random& rng)
  {
    T s = (T)(std::sqrt(var)*rng.normal());
    vector_ res(T(0));
    res[index] = s;
    res += var_from_dist<T,vector_,n,index-1>::value(var, rng);
    return res;
  }
};

//: base case
//  This is partial specialization
template <class T, class vector_, unsigned n>
struct var_from_dist<T,vector_,n,0>
{
  static inline vector_ value(const T& var, vnl_random& rng)
  {
    T s = (T)(std::sqrt(var)*rng.normal());
    vector_ res(T(0));
    res[0] = s;
    return res;
  };
};


//: base case
//  This is partial specialization
template <class T, class vector_>
struct var_from_dist<T,vector_,1,0>
{
  static inline vector_ value(const T& var, vnl_random& rng)
  {
    T s = (T)(std::sqrt(var)*rng.normal());
    vector_ res(s);
    return res;
  };
};


//: sample
template <class T, unsigned int n>
typename bsta_gaussian_sphere<T,n>::vector_ bsta_gaussian_sphere<T,n>::sample(vnl_random& rng) const
{
  vector_ mean = bsta_gaussian<T,n>::mean_;
  vector_ var = var_from_dist<T, vector_, n, n-1>::value(var_, rng);
  return mean+var;
}

#define BSTA_GAUSSIAN_SPHERE_INSTANTIATE(T,n) \
template class bsta_gaussian_sphere<T,n >


#endif // bsta_gaussian_sphere_hxx_
