// This is core/vpgl/vpgl_poly_radial_distortion.hxx
#ifndef vpgl_poly_radial_distortion_hxx_
#define vpgl_poly_radial_distortion_hxx_
//:
// \file

#include "vpgl_poly_radial_distortion.h"

// The templated helper functions are a metaprogram which allows the
// compiler to create a closed form (no loops) expression for polynomial
// evaluation of any order.  For very large n loops may be more efficient
// but for lens distortion the order is usually small.

template <class T, int n>
struct vpgl_poly_helper{
  static inline T val(const T& radius, const T* k)
  {
    return ((*k)+vpgl_poly_helper<T,n-1>::val(radius, k+1))*radius;
  }
};

template <class T>
struct vpgl_poly_helper<T,1>{
  static inline T val(const T& radius, const T* k)
  {
    return (*k)*radius;
  }
};


//: Distort a radial length
template <class T, int n>
T
vpgl_poly_radial_distortion<T,n>::distort_radius( T radius ) const
{
  return 1 + vpgl_poly_helper<T,n>::val(radius, coefficients_);
}


//====================================================================


template <class T, int n>
struct vpgl_poly_deriv_helper{
  static inline T val(const T& radius, const T* k, unsigned int p)
  {
    return p*(*k) + vpgl_poly_deriv_helper<T,n-1>::val(radius, k+1, p+1)*radius;
  }
};


template <class T>
struct vpgl_poly_deriv_helper<T,1>{
  static inline T val(const T& /*radius*/, const T* k, unsigned int p)
  {
    return p*(*k);
  }
};

//: Compute the derivative of the distort_radius function
template <class T, int n>
T
vpgl_poly_radial_distortion<T,n>::distort_radius_deriv( T radius ) const
{
  return vpgl_poly_deriv_helper<T,n>::val(radius, coefficients_, 1);
}


// Code for easy instantiation.
#undef vpgl_POLY_RADIAL_DISTORTION_INSTANTIATE
#define vpgl_POLY_RADIAL_DISTORTION_INSTANTIATE(T,n) \
template class vpgl_poly_radial_distortion<T,n>

#endif // vpgl_poly_radial_distortion_hxx_
