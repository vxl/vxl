// This is core/vpgl/vpgl_radial_tangential_distortion.hxx
#ifndef vpgl_radial_tangential_distortion_hxx_
#define vpgl_radial_tangential_distortion_hxx_
//:
// \file

#include <cmath>
#include <limits>
#include <iostream>
#include "vpgl_radial_tangential_distortion.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
template <class T>
vgl_vector_2d<T>
vpgl_radial_tangential_distortion<T>::apply_distortion(const vgl_vector_2d<T> & x) const
{
  T rsq = x.length(); // radial distance
  rsq *= rsq;
  T xi = x.x(), yi = x.y(); // projection onto the image plane

  size_t max_k = k_.size();
  T num = T(1);
  T den = T(1);
  T rpow = rsq;

  if (max_k <= 3)
  {
    for (size_t k = 0; k < max_k; ++k)
    {
      num += k_[k] * rpow;
      rpow *= rsq;
    }
  }
  else if (max_k > 3 && max_k <= 6)
  {
    for (size_t k = 0; k < 3; ++k)
    {
      num += k_[k] * rpow;
      rpow *= rsq;
    }
    rpow = rsq;
    for (size_t k = 3; k < max_k; ++k)
    {
      den += k_[k] * rpow;
      rpow *= rsq;
    }
  }
  else
  {
    std::cout << "more than 6 radial distortion coefficients - not valid  nk = " << max_k << std::endl;
    return vgl_vector_2d<T>(T(0), T(0));
  }
  T fr = num / den;
  T x_xp = (xi * fr) + T(2) * p1_ * xi * yi + p2_ * (rsq + T(2) * xi * xi);
  T x_yp = (yi * fr) + p1_ * (rsq + T(2) * yi * yi) + T(2) * p2_ * xi * yi;

  return vgl_vector_2d<T>(x_xp, x_yp);
}
//: Distort a projected point on the image plane
//  Calls the pure virtual radial distortion function
template <class T>
vgl_homg_point_2d<T>
vpgl_radial_tangential_distortion<T>::distort(const vgl_homg_point_2d<T> & point) const
{
  vgl_vector_2d<T> x = vgl_point_2d<T>(point) - center_;
  vgl_vector_2d<T> xp = apply_distortion(x);
  return vgl_homg_point_2d<T>(center_ + xp);
}


//: Return the original point that was distorted to this location (inverse of distort)
// \param init is an initial guess at the solution for the iterative solver
// if \p init is NULL then \p point is used as the initial guess
// calls the radial undistortion function
template <class T>
vgl_homg_point_2d<T>
vpgl_radial_tangential_distortion<T>::undistort(const vgl_homg_point_2d<T> & point,
                                                const vgl_homg_point_2d<T> * init) const
{
  vgl_point_2d<T> p(point);
  vgl_vector_2d<T> pr = p - center_;
  vgl_vector_2d<T> p0 = p - center_;
  if (init)
    p0 = vgl_point_2d<T>(*init) - center_;

  // uses the Newton Method with finite differences for root finding
  T eps = T(100) * std::numeric_limits<T>::epsilon();
  T delta = 0.0001;
  vgl_vector_2d<T> dpx(delta, T(0));
  vgl_vector_2d<T> dpy(T(0), delta);
  T large = std::numeric_limits<T>::max();
  vgl_vector_2d<T> del(large, large);
  unsigned int i = 0;
  for (; i < 100 && fabs(del.x()) > eps && fabs(del.y()) > eps; ++i)
  {
    vgl_vector_2d<T> v0 = apply_distortion(p0); // input point predicted from current solution
    del = pr - v0;                              // get the difference vector
    // compute derivatives for Taylor series
    vgl_vector_2d<T> dvx = (apply_distortion(p0 + dpx) - v0) / delta;
    vgl_vector_2d<T> dvy = (apply_distortion(p0 + dpy) - v0) / delta;
    // solve the linear system for the update to p0
    T det = dvx.x() * dvy.y() - dvx.y() * dvy.x();
    if (fabs(det) < eps)
    {
      std::cout << "singular system in undistort radial/tangential" << std::endl;
      return point;
    }
    // the delta for new undistorted point relative to the previous solution
    vgl_vector_2d<T> del_p0((dvy.y() * del.x() - dvx.y() * del.y()), dvx.x() * del.y() - dvy.x() * del.x());
#if 0
	std::cout << "pr (" << pr.x() << ' ' << pr.y() << " )" << std::endl;
	std::cout << "v0( " << v0.x() << ' ' << v0.y() << " )" << std::endl;
	std::cout << "pr-v0( " << del.x() << ' ' << del.y() << " )" << std::endl;
	std::cout << "dx'/dx " << dvx.x() << "  dx'/dy " << dvx.y() << std::endl;
	std::cout << "dy'/dx " << dvy.x() << "  dy'/dy " << dvy.y() << std::endl;
	std::cout << "det " << det << std::endl;
#endif
    del_p0 /= det;
    p0 += del_p0;
#if 0
	std::cout << "p-p0 " << del_p0.x() << ' ' << del_p0.y() << " )" << std::endl;
	std::cout << "v0+ (" << p0.x() << ' ' << p0.y() << " )" << std::endl;
	std::cout << "pr (" << pr.x() << ' ' << pr.y() << " )" << std::endl;
#endif
  }
  if (i >= 100)
  {
    std::cout << "Newton's methhod failed to converge in undistort" << std::endl;
    return vgl_homg_point_2d<T>(0.0, 0.0, 0.0); // ideal point
  }
  vgl_point_2d<T> temp = center_ + p0;
  return vgl_homg_point_2d<T>(temp);
}

// Code for easy instantiation.
#undef vpgl_RADIAL_TANGENTIAL_DISTORTION_INSTANTIATE
#define vpgl_RADIAL_TANGENTIAL_DISTORTION_INSTANTIATE(T) template class vpgl_radial_tangential_distortion<T>

#endif // vpgl_radial_tangential_distortion_hxx_
