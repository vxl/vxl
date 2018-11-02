// This is core/vgl/vgl_homg_line_3d_2_points.hxx
#ifndef vgl_homg_line_3d_2_points_hxx_
#define vgl_homg_line_3d_2_points_hxx_
//:
// \file

#include <iostream>
#include "vgl_homg_line_3d_2_points.h"
#include "vgl_tolerance.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

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
  //Require tolerance on ideal point
  Type tol = vgl_tolerance<Type>::position;
  if (point_infinite_.w() < tol && point_infinite_.w()>-tol) return; // already OK
  else if (point_finite_.w() < tol && point_finite_.w()>-tol) // interchange the points
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

//: Return the intersection point of two concurrent lines
template <class Type>
vgl_homg_point_3d<Type> intersection(vgl_homg_line_3d_2_points<Type> const& l1, vgl_homg_line_3d_2_points<Type> const& l2)
{
  assert(concurrent(l1,l2));
  Type a0=l1.point_finite().x(), a1=l1.point_infinite().x(), a2=l2.point_finite().x(), a3=l2.point_infinite().x(),
       b0=l1.point_finite().y(), b1=l1.point_infinite().y(), b2=l2.point_finite().y(), b3=l2.point_infinite().y(),
       c0=l1.point_finite().z(), c1=l1.point_infinite().z(), c2=l2.point_finite().z(), c3=l2.point_infinite().z(),
       d0=l1.point_finite().w(), d1=l1.point_infinite().w(), d2=l2.point_finite().w(), d3=l2.point_infinite().w();
  Type t1 = b3*a1-a3*b1, t2 = (a2-a0)*b3-(b2-b0)*a3;
  if (t1==0 && t2==0)
    t1 = c3*a1-a3*c1, t2 = (a2-a0)*c3-(c2-c0)*a3;
  if (t1==0 && t2==0)
    t1 = d3*a1-a3*d1, t2 = (a2-a0)*d3-(d2-d0)*a3;
  if (t1==0 && t2==0)
    t1 = c3*b1-b3*c1, t2 = (b2-b0)*c3-(c2-c0)*b3;
  if (t1==0 && t2==0)
    t1 = d3*b1-b3*d1, t2 = (b2-b0)*d3-(d2-d0)*b3;
  if (t1==0 && t2==0)
    t1 = d3*c1-c3*d1, t2 = (c2-c0)*d3-(d2-d0)*c3;
  return vgl_homg_point_3d<Type>(t1*a0+t2*a1,t1*b0+t2*b1,t1*c0+t2*c1,t1*d0+t2*d1);
}

//*****************************************************************************
// stream operators
//*****************************************************************************

template <class Type>
std::ostream& operator<<(std::ostream &s,
                        const vgl_homg_line_3d_2_points<Type> &p)
{
  return s << "<vgl_homg_line_3d_2_points "
           << p.point_finite() << p.point_infinite() << " >";
}

#undef VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE
#define VGL_HOMG_LINE_3D_2_POINTS_INSTANTIATE(T) \
template class vgl_homg_line_3d_2_points<T >;\
template std::ostream& operator<<(std::ostream&, vgl_homg_line_3d_2_points<T > const&);\
template vgl_homg_point_3d<T > intersection(vgl_homg_line_3d_2_points<T > const&, vgl_homg_line_3d_2_points<T > const&)

#endif // vgl_homg_line_3d_2_points_hxx_
