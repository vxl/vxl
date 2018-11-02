// This is core/vgl/vgl_homg_plane_3d.hxx
#ifndef vgl_homg_plane_3d_hxx_
#define vgl_homg_plane_3d_hxx_
//:
// \file

#include <cmath>
#include <iostream>
#include "vgl_homg_plane_3d.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>

//: Construct from non-homogeneous plane
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d(vgl_plane_3d<Type> const& pl)
  : a_(pl.a()), b_(pl.b()), c_(pl.c()), d_(pl.d()) {}

//: Construct from three points
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d (vgl_homg_point_3d<Type> const& p1,
                                            vgl_homg_point_3d<Type> const& p2,
                                            vgl_homg_point_3d<Type> const& p3)
: a_(p1.w()*(p2.y()*p3.z()-p2.z()*p3.y())
    +p2.w()*(p3.y()*p1.z()-p3.z()*p1.y())
    +p3.w()*(p1.y()*p2.z()-p1.z()*p2.y()))
, b_(p1.w()*(p2.z()*p3.x()-p2.x()*p3.z())
    +p2.w()*(p3.z()*p1.x()-p3.x()*p1.z())
    +p3.w()*(p1.z()*p2.x()-p1.x()*p2.z()))
, c_(p1.w()*(p2.x()*p3.y()-p2.y()*p3.x())
    +p2.w()*(p3.x()*p1.y()-p3.y()*p1.x())
    +p3.w()*(p1.x()*p2.y()-p1.y()*p2.x()))
, d_(p1.x()*(p2.z()*p3.y()-p2.y()*p3.z())
    +p2.x()*(p1.y()*p3.z()-p1.z()*p3.y())
    +p3.x()*(p1.z()*p2.y()-p1.y()*p2.z()))
{
  assert(a_||b_||c_||d_); // points should not be collinear or coinciding
}

//: Construct from two concurrent lines
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d(vgl_homg_line_3d_2_points<Type> const& l1,
                                           vgl_homg_line_3d_2_points<Type> const& l2)
{
  assert(concurrent(l1,l2));
  vgl_homg_point_3d<Type> p1 = l1.point_finite();
  vgl_homg_point_3d<Type> p2 = l1.point_infinite();
  vgl_homg_point_3d<Type> p3 = l2.point_finite();
  if (collinear(p1,p2,p3)) p3 = l2.point_infinite();
  *this = vgl_homg_plane_3d<Type>(p1,p2,p3);
}

//: Construct from normal and a point
// This will fail when the given point is at infinity!
template <class Type>
vgl_homg_plane_3d<Type>::vgl_homg_plane_3d(vgl_vector_3d<Type> const& n,
                                           vgl_homg_point_3d<Type> const&p)
 : a_(n.x()*p.w()), b_(n.y()*p.w()), c_(n.z()*p.w()),
   d_(-(n.x()*p.x()+n.y()*p.y()+n.z()*p.z())) {}

//: divide all coefficients by sqrt(a^2 + b^2 + c^2)
template <class Type>
void vgl_homg_plane_3d<Type>::normalize()
{
  double sum = a_*a_ + b_*b_ + c_*c_;
  if (sum<1e-12) // don't normalize plane at infinity
    return;
  double den = std::sqrt(sum);
  double an= (double)a()/den; a_ = (Type)an;
  double bn= (double)b()/den; b_ = (Type)bn;
  double cn= (double)c()/den; c_ = (Type)cn;
  double dn= (double)d()/den; d_ = (Type)dn;
  //standardize so that largest of (|a|,|b|,|c|) is positive
  if ((std::fabs(an)>=std::fabs(bn) && std::fabs(an)>=std::fabs(cn) && an < 0) ||
      (std::fabs(bn)>std::fabs(an) && std::fabs(bn)>=std::fabs(cn) && bn < 0) ||
      (std::fabs(cn)>std::fabs(an) && std::fabs(cn)>std::fabs(bn) && cn < 0))
    a_ = -a_, b_ = -b_, c_ = -c_, d_ = -d_;
  return;
}

template <class Type>
bool vgl_homg_plane_3d<Type>::operator==(vgl_homg_plane_3d<Type> const& p) const
{
  return (this==&p) ||
         (   (a()*p.b()==p.a()*b())
          && (a()*p.c()==p.a()*c())
          && (a()*p.d()==p.a()*d())
          && (b()*p.c()==p.b()*c())
          && (b()*p.d()==p.b()*d())
          && (c()*p.d()==p.c()*d()) );
}

template <class Type>
std::ostream& operator<<(std::ostream& s, const vgl_homg_plane_3d<Type>& p)
{
  return s << " <vgl_homg_plane_3d "
           << p.a() << " x + "
           << p.b() << " y + "
           << p.c() << " z + "
           << p.d() << " w = 0 >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_homg_plane_3d<Type>& p)
{
  Type a, b, c, d;
  s >> a >> b >> c >> d;
  p.set(a,b,c,d);
  return s;
}

#undef VGL_HOMG_PLANE_3D_INSTANTIATE
#define VGL_HOMG_PLANE_3D_INSTANTIATE(T) \
template class vgl_homg_plane_3d<T >; \
template std::ostream& operator<<(std::ostream&, vgl_homg_plane_3d<T >const&); \
template std::istream& operator>>(std::istream&, vgl_homg_plane_3d<T >&)

#endif // vgl_homg_plane_3d_hxx_
