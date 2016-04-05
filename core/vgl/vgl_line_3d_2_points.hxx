// This is core/vgl/vgl_line_3d_2_points.hxx
#ifndef vgl_line_3d_2_points_hxx_
#define vgl_line_3d_2_points_hxx_
//:
// \file

#include <iostream>
#include <limits>
#include "vgl_line_3d_2_points.h"
// not used? #include <vcl_compiler.h>

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
std::ostream& operator<<(std::ostream &s,
                        const vgl_line_3d_2_points<Type> &p)
{
  return s << "<vgl_line_3d_2_points "
           << p.point1() << p.point2() << " >";
}

#undef VGL_LINE_3D_2_POINTS_INSTANTIATE
#define VGL_LINE_3D_2_POINTS_INSTANTIATE(T) \
template class vgl_line_3d_2_points<T >;\
template std::ostream& operator<<(std::ostream&, vgl_line_3d_2_points<T > const&)

#endif // vgl_line_3d_2_points_hxx_
