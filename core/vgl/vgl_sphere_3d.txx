#ifndef vgl_sphere_3d_txx_
#define vgl_sphere_3d_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \brief a sphere in 3D nonhomogeneous space
// \author Ian Scott

#include "vgl_sphere_3d.h"
#include <vcl_cmath.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_line_3d_2_points.h>


//: Return true iff the point p is inside (or on) this sphere
template <class T>
bool vgl_sphere_3d<T>::contains(vgl_point_3d<T> const& p) const
{
  return (p-c_).length() <= r_;
}


//: Calculate the end points of a line clipped by this sphere.
// \return true if any of the line touches the sphere.
template <class T>
bool vgl_sphere_3d<T>::clip(const vgl_line_3d_2_points<T> & line,
  vgl_point_3d<T> &p1, vgl_point_3d<T> &p2)
{
  vgl_point_3d<T> cp = vgl_closest_point(line, c_);

  T cp_len = (cp - c_).length();
  if (cp_len > r_) return false;

  T half_chord_len = vcl_sqrt(r_*r_ - cp_len*cp_len);

  vgl_vector_3d<T> linevec = line.direction();
  linevec *= half_chord_len / linevec.length();

  p1 = cp - linevec;
  p2 = cp + linevec;

  return true;
}

#undef VGL_SPHERE_3D_INSTANTIATE
#define VGL_SPHERE_3D_INSTANTIATE(Type) \
template class vgl_sphere_3d<Type >

#endif // vgl_sphere_3d_txx_
