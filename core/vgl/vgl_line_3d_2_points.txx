// This is core/vgl/vgl_line_3d_2_points.txx
#ifndef vgl_line_3d_2_points_txx_
#define vgl_line_3d_2_points_txx_
//:
// \file

#include "vgl_line_3d_2_points.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vcl_deprecated.h>

#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>

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
  VXL_DEPRECATED( "vgl_line_3d_2_points::intersection(vgl_line_3d_2_points<Type> const&, vgl_line_3d_2_points<Type> const&) -- Please use the method in vgl_intersection.h" );
  return vgl_intersection(l1, l2);
}

//: Return the intersection point of a line and a plane.
// \relates vgl_line_3d_2_points
// \relates vgl_plane_3d
template <class Type>
vgl_point_3d<Type> intersection(vgl_line_3d_2_points<Type> const& line,
                                vgl_plane_3d<Type> const& plane)
{
  VXL_DEPRECATED( "vgl_line_3d_2_points::intersection(vgl_line_3d_2_points<Type> const&, vgl_plane_3d<Type> const&) -- Please use the method in vgl_intersection.h" );
  return vgl_intersection(line, plane);
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
template vgl_point_3d<T > intersection(vgl_line_3d_2_points<T > const& l1, vgl_line_3d_2_points<T > const& l2);\
template vgl_point_3d<T > intersection(vgl_line_3d_2_points<T > const& , vgl_plane_3d<T > const&)

#endif // vgl_line_3d_2_points_txx_
