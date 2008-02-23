// This is brl/bbas/bsta/bsta_gaussian_sphere.txx
#ifndef bsta_gaussian_sphere_txx_
#define bsta_gaussian_sphere_txx_
//:
// \file

#include "bsta_gaussian_sphere.h"
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vnl/vnl_erf.h>

namespace {

//: Unroll the mahalanobis distance calculation
template <class T, unsigned n, unsigned index>
struct compute_dot
{
  static inline T value(const vnl_vector_fixed<T,n>& d)
  {
    return d[index-1]*d[index-1]
         + compute_dot<T,n,index-1>::value(d);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n>
struct compute_dot<T,n,0>
{
  static inline T value(const vnl_vector_fixed<T,n>& d)
  { return 0; }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T>
    struct compute_dot<T,1,1>
{
  static inline T value(const T& d)
  { return d*d; }
};


//: Unroll the determinant calculation
template <class T, unsigned n, unsigned index>
struct determinant
{
  static inline T value(const T& var)
  {
    return var * determinant<T,n,index-1>::value(var);
  }
};

//: base case
// this is partial specialization: expect MSVC6 to complain
template <class T, unsigned n>
struct determinant<T,n,0>
{
  static inline T value(const T& var)
  { return 1; }
};

};


//: The squared Mahalanobis distance to this point
template <class T, unsigned int n>
T
bsta_gaussian_sphere<T,n>::sqr_mahalanobis_dist(const vector_& pt) const
{
  if (var_<=T(0))
    return vcl_numeric_limits<T>::infinity();
  vector_ d = bsta_gaussian<T,n>::mean_-pt;
  return compute_dot<T,n,n>::value(d)/var_;
}


//: Unrol the compute probability calculation
//  The general induction step
template <class T, class vector_, unsigned n, unsigned index>
struct compute_probability_box
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var
                        )
  {
    if (var<=T(0))
      return vcl_numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/vcl_sqrt(sigma_sq_2);
    double temp = vnl_erf(max_minus_mean[index]*s2);
    temp -= vnl_erf(min_minus_mean[index]*s2);
    T res = static_cast<T>(0.5*temp);
    res *= compute_probability_box<T,vector_,n,index-1>::value(min_minus_mean,
                                                               max_minus_mean,
                                                               var);
    return res;
  }
};

//: base case
//  This is partial specialization: expect MSVC6 to complain
template <class T, class vector_, unsigned n>
struct compute_probability_box<T,vector_,n,0>
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var)
  {
    if (var<=T(0))
      return vcl_numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/vcl_sqrt(sigma_sq_2);
    double temp = vnl_erf(max_minus_mean[0]*s2);
    temp -= vnl_erf(min_minus_mean[0]*s2);
    return static_cast<T>(0.5*temp);
  };
};


//: base case
//  This is partial specialization: expect MSVC6 to complain
template <class T, class vector_>
struct compute_probability_box<T,vector_,1,0>
{
  static inline T value(const vector_& min_minus_mean,
                        const vector_& max_minus_mean,
                        const T& var)
  {
    if (var<=T(0))
      return vcl_numeric_limits<T>::infinity();
    double sigma_sq_2 = 2.0*static_cast<double>(var);
    double s2 = 1/vcl_sqrt(sigma_sq_2);
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
  return compute_probability_box<T, vector_, n, n-1>::value(min_minus_mean,
                                                            max_minus_mean,
                                                            var_);
}

//: The determinant of the covariance matrix
template <class T, unsigned int n>
void
bsta_gaussian_sphere<T,n>::compute_det()
{
  det_covar_ = determinant<T,n,n>::value(var_);
  assert(det_covar_ >= 0);
}


#define BSTA_GAUSSIAN_SPHERE_INSTANTIATE(T,n) \
template class bsta_gaussian_sphere<T,n >


#endif // bsta_gaussian_sphere_txx_
