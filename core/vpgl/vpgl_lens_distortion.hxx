// This is core/vpgl/vpgl_lens_distortion.hxx
#ifndef vpgl_lens_distortion_hxx_
#define vpgl_lens_distortion_hxx_
//:
// \file

#include "vpgl_lens_distortion.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>
#if 0
#  include <vgl/vgl_vector_2d.h>
#endif
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <class T>
vgl_homg_point_2d<T>
vpgl_lens_distortion<T>::distort_pixel(const vgl_homg_point_2d<T> & pixel, const vpgl_calibration_matrix<T> & K) const
{
  // convert image coordinates to focal plane coordinates
  T f = K.focal_length();
  T ax = K.x_scale(), ay = K.y_scale();
  vgl_point_2d<T> pp = K.principal_point();
  T sk = K.skew();
  vgl_point_2d<T> c(pp.x(), pp.y());
  vgl_point_2d<T> pix(pixel), pix_d;
  // apply the inverse of the calibration matrix
  vgl_vector_2d<T> xvu = pix - c, xv;
  vgl_homg_point_2d<T> hux, hx;
  if (sk == T(0))
    hux.set(xvu.x() / (ax * f), xvu.y() / (ay * f));
  else
    hux.set((xvu.x() / (ax * f) - sk * xvu.y() / (ax * ay * f * f)), xvu.y() / (ay * f));

  // hux is now in focal plane coordinates so distortion can be applied
  hx = this->distort(hux);

  // convert back to pixel coordinates
  if (sk == T(0))
    xv.set(hx.x() * (ax * f), hx.y() * (ay * f));
  else
    xv.set((hx.x() * (ax * f) + sk * hx.y()), hx.y() * (ay * f));
  pix_d = c + xv;
  return vgl_homg_point_2d<T>(pix_d.x(), pix_d.y());
}


template <class T>
vgl_homg_point_2d<T>
vpgl_lens_distortion<T>::undistort_pixel(const vgl_homg_point_2d<T> & distorted_pixel,
                                         const vpgl_calibration_matrix<T> & K) const
{
  // convert image coordinates to focal plane coordinates
  T f = K.focal_length();
  T ax = K.x_scale(), ay = K.y_scale();
  vgl_point_2d<T> pp = K.principal_point();
  T sk = K.skew();
  vgl_point_2d<T> c(pp.x(), pp.y());
  vgl_point_2d<T> pix_d(distorted_pixel), pix_ud;
  // apply the inverse of the calibration matrix
  vgl_vector_2d<T> xv = pix_d - c, xvu;
  vgl_homg_point_2d<T> hx, hux;
  if (sk == T(0))
    hx.set(xv.x() / (ax * f), xv.y() / (ay * f));
  else
    hx.set((xv.x() / (ax * f) - sk * xv.y() / (ax * ay * f * f)), xv.y() / (ay * f));

  // hx is now in focal plane coordinates so undistort
  hux = this->undistort(hx);

  // convert back to pixel coordinates
  if (sk == T(0))
    xvu.set(hux.x() * (ax * f), hux.y() * (ay * f));
  else
    xvu.set((hux.x() * (ax * f) + sk * hux.y()), hux.y() * (ay * f));
  pix_ud = c + xvu;
  return vgl_homg_point_2d<T>(pix_ud.x(), pix_ud.y());
}

// Code for easy instantiation.
#undef vpgl_LENS_DISTORTION_INSTANTIATE
#define vpgl_LENS_DISTORTION_INSTANTIATE(T) template class vpgl_lens_distortion<T>

#endif // vpgl_lens_distortion_hxx_
