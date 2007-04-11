// This is core/vgl/vgl_line_3d_2_points.txx
#ifndef vgl_line_3d_2_points_txx_
#define vgl_line_3d_2_points_txx_
//:
// \file

#include "vgl_line_3d_2_points.h"
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_limits.h>

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
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_3d_2_points<T > const&)

#endif // vgl_line_3d_2_points_txx_
