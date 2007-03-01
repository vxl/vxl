// This is core/vgl/vgl_intersection.txx
#ifndef vgl_intersection_txx_
#define vgl_intersection_txx_
//:
// \file
// \author Gamze Tunali

#include "vgl_intersection.h"
#include <vcl_algorithm.h>
#include <vcl_limits.h>
#include <vcl_cassert.h>

#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_distance.h>


template <class T>
vgl_box_3d<T> vgl_intersection(vgl_box_3d<T> const& a, vgl_box_3d<T> const& b)
{
  T x0 = vcl_max(a.min_x(), b.min_x());
  T y0 = vcl_max(a.min_y(), b.min_y());
  T z0 = vcl_max(a.min_z(), b.min_z());
  T x1 = vcl_min(a.max_x(), b.max_x());
  T y1 = vcl_min(a.max_y(), b.max_y());
  T z1 = vcl_min(a.max_z(), b.max_z());

  if (x1 >= x0 && y1 >= y0 && z1 >= z0)
    return vgl_box_3d<T>(x0, y0, z0, x1, y1, z1);
  else
    return vgl_box_3d<T>(); // empty box
}

//: Return the intersection point of two concurrent lines
template <class T>
vgl_point_3d<T> vgl_intersection(vgl_line_3d_2_points<T> const& l1,
                             vgl_line_3d_2_points<T> const& l2)
{
  assert(concurrent(l1,l2));
  T a0=l1.point1().x(), a1=l1.point2().x(), a2=l2.point1().x(), a3=l2.point2().x(),
       b0=l1.point1().y(), b1=l1.point2().y(), b2=l2.point1().y(), b3=l2.point2().y(),
       c0=l1.point1().z(), c1=l1.point2().z(), c2=l2.point1().z(), c3=l2.point2().z();
  T t1 = (b3-b2)*(a1-a0)-(a3-a2)*(b1-b0), t2 = (b0-b2)*(a1-a0)-(a0-a2)*(b1-b0);
  if (t1 == 0)
       t1 = (c3-c2)*(a1-a0)-(a3-a2)*(c1-c0), t2 = (c0-c2)*(a1-a0)-(a0-a2)*(c1-c0);
  if (t1 == 0)
       t1 = (c3-c2)*(b1-b0)-(b3-b2)*(c1-c0), t2 = (c0-c2)*(b1-b0)-(b0-b2)*(c1-c0);
  return vgl_point_3d<T>(((t1-t2)*a2+t2*a3)/t1,
                            ((t1-t2)*b2+t2*b3)/t1,
                            ((t1-t2)*c2+t2*c3)/t1);
}

//: Return the intersection point of a line and a plane.
// \relates vgl_line_3d_2_points
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(vgl_line_3d_2_points<T> const& line,
                                vgl_plane_3d<T> const& plane)
{
  vgl_vector_3d<T> dir = line.direction();

  vgl_point_3d<T> pt;

  double denom = plane.a()*(dir.x()) +
                 plane.b()*(dir.y()) +
                 plane.c()*(dir.z());

  if (denom == 0)
  {
    const T inf = vcl_numeric_limits<T>::infinity();
    // Line is either parallel or coplanar
    // If the distance from a line endpoint to the plane is zero, coplanar
    if (vgl_distance(line.point1(), plane)==0.0)
      pt.set(inf,inf,inf);
    else
      pt.set(inf,0,0);
  }
  else
  {
    // Infinite line intersects plane
    double numer = -(plane.a()*line.point1().x() +
      plane.b()*line.point1().y() +
      plane.c()*line.point1().z() +
      plane.d());

    dir *= numer/denom;
    pt = line.point1() + dir;
  }

  return pt;
}

//: Return the intersection point of three planes.
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(const vgl_plane_3d<T>& p1,
                             const vgl_plane_3d<T>& p2,
                             const vgl_plane_3d<T>& p3)
{
  vgl_point_3d<T> p(p1, p2, p3);
  return p;
}

#undef VGL_INTERSECTION_INSTANTIATE
#define VGL_INTERSECTION_INSTANTIATE(T) \
template vgl_box_3d<T > vgl_intersection(vgl_box_3d<T > const&, vgl_box_3d<T > const&);\
template vgl_point_3d<T > vgl_intersection(vgl_line_3d_2_points<T > const&,vgl_line_3d_2_points<T > const&);\
template vgl_point_3d<T > vgl_intersection(vgl_line_3d_2_points<T > const&,vgl_plane_3d<T > const&);\
template vgl_point_3d<T > vgl_intersection(const vgl_plane_3d<T >&,const vgl_plane_3d<T >&,const vgl_plane_3d<T >&)

#endif // vgl_intersection_txx_
