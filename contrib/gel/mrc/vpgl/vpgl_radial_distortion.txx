// This is gel/mrc/vpgl/vpgl_radial_distortion.txx
#ifndef vpgl_radial_distortion_txx_
#define vpgl_radial_distortion_txx_
//:
// \file

#include "vpgl_radial_distortion.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>

#include <vcl_cmath.h>
// not used? #include <vcl_iostream.h>
#include <vcl_limits.h>

//: Distort a projected point on the image plane
//  Calls the pure virtual radial distortion function
template <class T>
vgl_homg_point_2d<T>
vpgl_radial_distortion<T>::distort( const vgl_homg_point_2d<T>& point ) const
{
  vgl_vector_2d<T> r = vgl_point_2d<T>(point) - center_;
  T scale = distort_radius(r.length());
  return vgl_homg_point_2d<T>(distorted_center_ + scale*r);
}


//: Return the original point that was distorted to this location (inverse of distort)
// \param init is an initial guess at the solution for the iterative solver
// if \p init is NULL then \p point is used as the initial guess
// calls the radial undistortion function
template <class T>
vgl_homg_point_2d<T>
vpgl_radial_distortion<T>::undistort( const vgl_homg_point_2d<T>& point,
                                       const vgl_homg_point_2d<T>* init ) const
{
  vgl_vector_2d<T> r = vgl_point_2d<T>(point) - distorted_center_;
  T radius = r.length();
  T init_r = radius;
  if (init)
    init_r = (vgl_point_2d<T>(*init) - center_).length();
  T scale = undistort_radius(radius, &init_r);
  return vgl_homg_point_2d<T>(center_ + scale*r);
}


//: Return the inverse of the distort function
// \param init is an initial guess at the solution for the iterative solver
// if \p init is NULL then \p radius is used as the initial guess
template <class T>
T
vpgl_radial_distortion<T>::undistort_radius( T radius, const T* init) const
{
  if (radius == T(0))
    return T(1);

  T result = radius;
  if (init)
    result = *init;

  if (has_derivative_){
    // uses the Newton Method for root finding
    T e = vcl_numeric_limits<T>::infinity();
    T eps = vcl_numeric_limits<T>::epsilon();
    for (unsigned int i=0; i<100 && vcl_abs(e)>eps ; ++i){
      T f_result = distort_radius(result);
      e = radius - f_result*result;
      result += e/(distort_radius_deriv(result)*result + f_result);
    }
  }
  else{
    // uses the Newton Method with finite differences for root finding
    T e = vcl_numeric_limits<T>::infinity();
    T eps = vcl_numeric_limits<T>::epsilon();
    T df = T(0.001);
    for (unsigned int i=0; i<100 && vcl_abs(e)>eps ; ++i){
      T f_result = distort_radius(result);
      T f_result_df = distort_radius(result-df);
      e = radius - f_result*result;
      result += e/((f_result - f_result_df)*result/df + f_result);
    }
  }

  return result/radius;
}

// Code for easy instantiation.
#undef vpgl_RADIAL_DISTORTION_INSTANTIATE
#define vpgl_RADIAL_DISTORTION_INSTANTIATE(T) \
template class vpgl_radial_distortion<T >

#endif // vpgl_radial_distortion_txx_
