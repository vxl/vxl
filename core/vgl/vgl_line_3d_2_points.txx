// This is vxl/vgl/vgl_line_3d_2_points.txx
#ifndef vgl_line_3d_2_points_txx_
#define vgl_line_3d_2_points_txx_
//:
// \file

#include "vgl_line_3d_2_points.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>

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
  Type t1 = b3*a1-a3*b1, t2 = (a2-a0)*b3-(b2-b0)*a3;
  if (t1==0 && t2==0)
    t1 = c3*a1-a3*c1, t2 = (a2-a0)*c3-(c2-c0)*a3;
  if (t1==0 && t2==0)
    t1 = a1-a3, t2 = a2-a0;
  if (t1==0 && t2==0)
    t1 = c3*b1-b3*c1, t2 = (b2-b0)*c3-(c2-c0)*b3;
  if (t1==0 && t2==0)
    t1 = b1-b3, t2 = b2-b0;
  if (t1==0 && t2==0)
    t1 = c1-c3, t2 = c2-c0;
  return vgl_point_3d<Type>((t1*a0+t2*a1)/(t1+t2),
                            (t1*b0+t2*b1)/(t1+t2),
                            (t1*c0+t2*c1)/(t1+t2));
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
template vgl_point_3d<T > intersection(vgl_line_3d_2_points<T > const&, vgl_line_3d_2_points<T > const&)

#endif // vgl_line_3d_2_points_txx_
