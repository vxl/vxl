// This is bbas/bpgl/bpgl_poly_radial_distortion.txx
#ifndef bpgl_poly_radial_distortion_txx_
#define bpgl_poly_radial_distortion_txx_
//:
// \file

#include "bpgl_poly_radial_distortion.h"

#if VCL_CAN_DO_PARTIAL_SPECIALIZATION

// The templated helper functions are a metaprogram which allows the
// compiler to create a closed form (no loops) expression for polynomial
// evaluation of any order.  For very large n loops may be more efficient
// but for lens distortion the order is usually small.

template <class T, int n>
struct bpgl_poly_helper{
  static inline T val(const T& radius, const T* k)
  {
    return ((*k)+bpgl_poly_helper<T,n-1>::val(radius, k+1))*radius;
  }
};

template <class T>
struct bpgl_poly_helper<T,1>{
  static inline T val(const T& radius, const T* k)
  {
    return (*k)*radius;
  }
};


//: Distort a radial length
template <class T, int n>
T
bpgl_poly_radial_distortion<T,n>::distort_radius( T radius ) const
{
  return 1 + bpgl_poly_helper<T,n>::val(radius, coefficients_);
}


//====================================================================


template <class T, int n>
struct bpgl_poly_deriv_helper{
  static inline T val(const T& radius, const T* k, unsigned int p)
  {
    return p*(*k) + bpgl_poly_deriv_helper<T,n-1>::val(radius, k+1, p+1)*radius;
  }
};


template <class T>
struct bpgl_poly_deriv_helper<T,1>{
  static inline T val(const T& /*radius*/, const T* k, unsigned int p)
  {
    return p*(*k);
  }
};

//: Compute the derivative of the distort_radius function
template <class T, int n>
T
bpgl_poly_radial_distortion<T,n>::distort_radius_deriv( T radius ) const
{
  return bpgl_poly_deriv_helper<T,n>::val(radius, coefficients_, 1);
}


#else // VCL_CAN_DO_PARTIAL_SPECIALIZATION

// If we can not use partial template instantiation we loop instead.

//: Distort a radial length
template <class T, int n>
T
bpgl_poly_radial_distortion<T,n>::distort_radius( T radius ) const
{
  T value = T(0);
  for (int i=n-1; i>=0; --i){
    value = (coefficients_[i]+value)*radius;
  }
  return 1 + value;
}

//: Compute the derivative of the distort_radius function
template <class T, int n>
T
bpgl_poly_radial_distortion<T,n>::distort_radius_deriv( T radius ) const
{
  T value = T(0);
  for (int i=n-1; i>=0; --i){
    value = (i+1)*coefficients_[i] + value*radius;
  }
  return value;
}

#endif // VCL_CAN_DO_PARTIAL_SPECIALIZATION

// Code for easy instantiation.
#undef BPGL_POLY_RADIAL_DISTORTION_INSTANTIATE
#define BPGL_POLY_RADIAL_DISTORTION_INSTANTIATE(T,n) \
template class bpgl_poly_radial_distortion<T,n>

#endif // bpgl_poly_radial_distortion_txx_
