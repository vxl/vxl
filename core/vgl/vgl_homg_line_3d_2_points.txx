// This is ./vxl/vgl/vgl_homg_line_3d_2_points.txx
#ifndef vgl_homg_line_3d_2_points_txx_
#define vgl_homg_line_3d_2_points_txx_

//:
// \file

#include <vcl_iostream.h>
#include "vgl_homg_line_3d_2_points.h"

//***************************************************************************
// Initialization
//***************************************************************************


//***************************************************************************
// Utility methods
//***************************************************************************

template <class Type>
bool vgl_homg_line_3d_2_points<Type>::operator==(vgl_homg_line_3d_2_points<Type> const& other) const
{
  if (this==&other)
    return true;
  if (!point_finite().ideal()) {
    if (point_infinite() != other.point_infinite())
      return false;
    if (point_finite() == other.point_finite())
      return true;
    // Now it suffices to check that the points are collinear:
    return collinear(point_infinite(), point_finite(), other.point_finite());
  }
  // and in the case of the line being at infinity:
  return collinear(point_infinite(), point_finite(), other.point_finite())
     &&  collinear(other.point_infinite(), point_finite(), other.point_finite());
}

//: force the point point_infinite_ to infinity, without changing the line
template <class Type>
void vgl_homg_line_3d_2_points<Type>::force_point2_infinite(void) const
{
  if (point_infinite_.w() == 0) return; // already OK
  else if (point_finite_.w() == 0) // interchange the points
  {
    vgl_homg_point_3d<Type> t=point_infinite_;
    point_infinite_=point_finite_;
    point_finite_=t;
    return;
  }
  Type a=point_finite_.x(), a1=point_infinite_.x(),
       b=point_finite_.y(), b1=point_infinite_.y(),
       c=point_finite_.z(), c1=point_infinite_.z(),
       d=point_finite_.w(), d1=point_infinite_.w();
  point_infinite_.set(a*d1-a1*d, b*d1-b1*d, c*d1-c1*d, 0);
}

//*****************************************************************************
// stream operators
//*****************************************************************************

template <class Type>
vcl_ostream& operator<<(vcl_ostream &s,
                        const vgl_homg_line_3d_2_points<Type> &p)
{
  return s << "<vgl_homg_line_3d_2_points "
           << p.point_finite() << p.point_infinite() << " >";
}

#undef VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE
#define VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE(T) \
template class vgl_homg_line_3d_2_points<T >;\
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_line_3d_2_points<T > const&)
#endif // vgl_homg_line_3d_2_points_txx_
