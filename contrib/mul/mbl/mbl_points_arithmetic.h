#ifndef mbl_points_arithmetic_h_
#define mbl_points_arithmetic_h_


//:
// \file
// \brief Functions for performing arithmetic on vgl_points_2d<>
// \author Ian Scott
// \date 28 June 2001

#include <vgl/vgl_point_2d.h>


template <class T>
vgl_point_2d<T> operator+ (const vgl_point_2d<T> &a, const vgl_point_2d<T> &b)
{
  return vgl_point_2d<T>(a.x() + b.x(), a.y() + b.y());
}

template <class T>
vgl_point_2d<T> operator- (const vgl_point_2d<T> &a, const vgl_point_2d<T> &b)
{
  return vgl_point_2d<T>(a.x() - b.x(), a.y() - b.y());
}

template <class T>
vgl_point_2d<T> operator* (T s, const vgl_point_2d<T> &b)
{
  return vgl_point_2d<T>(s * a.x(), s *a.y());
}

template <class T>
vgl_point_2d<T> operator* (const vgl_point_2d<T> &b, T s)
{
  return vgl_point_2d<T>(s * a.x(), s *a.y());
}

#endif