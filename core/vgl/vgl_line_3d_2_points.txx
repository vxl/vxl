// This is core/vgl/vgl_line_3d_2_points.txx
#ifndef vgl_line_3d_2_points_txx_
#define vgl_line_3d_2_points_txx_
//:
// \file

#include "vgl_line_3d_2_points.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>

//***************************************************************************
// Initialization
//***************************************************************************


//***************************************************************************
// Utility methods
//***************************************************************************

template <class Type>
bool vgl_line_3d_2_points<Type>::operator==(vgl_line_3d_2_points<Type> const& other) const
{
  if (this==&other)
    return true;
  // It suffices to check that the points are collinear:
  return collinear(point1(), point2(), other.point1())
      && collinear(point1(), point2(), other.point2());
}

//: Return the intersection point of two concurrent lines
template <class Type>
vgl_point_3d<Type> intersection(vgl_line_3d_2_points<Type> const& l1,
                                vgl_line_3d_2_points<Type> const& l2)
{
  assert(concurrent(l1,l2));
  Type a0=l1.point1().x(), a1=l1.point2().x(), a2=l2.point1().x(), a3=l2.point2().x(),
       b0=l1.point1().y(), b1=l1.point2().y(), b2=l2.point1().y(), b3=l2.point2().y(),
       c0=l1.point1().z(), c1=l1.point2().z(), c2=l2.point1().z(), c3=l2.point2().z();
  Type t1 = (b3-b2)*(a1-a0)-(a3-a2)*(b1-b0), t2 = (b0-b2)*(a1-a0)-(a0-a2)*(b1-b0);
  if (t1 == 0)
       t1 = (c3-c2)*(a1-a0)-(a3-a2)*(c1-c0), t2 = (c0-c2)*(a1-a0)-(a0-a2)*(c1-c0);
  if (t1 == 0)
       t1 = (c3-c2)*(b1-b0)-(b3-b2)*(c1-c0), t2 = (c0-c2)*(b1-b0)-(b0-b2)*(c1-c0);
  return vgl_point_3d<Type>(((t1-t2)*a2+t2*a3)/t1,
                            ((t1-t2)*b2+t2*b3)/t1,
                            ((t1-t2)*c2+t2*c3)/t1);
}


//: Return the intersection point of a line and a plane.
// \relates vgl_line_3d_2_points
// \relates vgl_plane_3d
template <class Type>
vgl_point_3d<Type> intersection(vgl_line_3d_2_points<Type> const& line,
                                vgl_plane_3d<Type> const& plane)
{
  vgl_vector_3d<Type> dir = line.direction();

  vgl_point_3d<Type> pt;

  double denom = plane.a()*(dir.x()) +
                 plane.b()*(dir.y()) +
                 plane.c()*(dir.z());

  if (denom == 0)
  {
    const Type inf = vcl_numeric_limits<Type>::infinity();
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


//*****************************************************************************
// stream operators
//*****************************************************************************

template <class Type>
vcl_ostream& operator<<(vcl_ostream &s,
                        const vgl_line_3d_2_points<Type> &p)
{
  return s << "<vgl_line_3d_2_points "
           << p.point1() << p.point2() << " >";
}

#undef VGL_LINE_3D_2_POINTS_INSTANTIATE
#define VGL_LINE_3D_2_POINTS_INSTANTIATE(T) \
template class vgl_line_3d_2_points<T >;\
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_3d_2_points<T > const&);\
template vgl_point_3d<T > intersection(vgl_line_3d_2_points<T > const&, vgl_line_3d_2_points<T > const&);\
template vgl_point_3d<T > intersection(vgl_line_3d_2_points<T > const&, vgl_plane_3d<T > const&)

#endif // vgl_line_3d_2_points_txx_
