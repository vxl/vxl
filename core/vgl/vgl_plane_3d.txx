// This is vxl/vgl/vgl_plane_3d.txx
#ifndef vgl_plane_3d_txx_
#define vgl_plane_3d_txx_
//:
// \file

#include "vgl_plane_3d.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_point_3d.h>

//: Construct from homogeneous plane
template <class Type>
vgl_plane_3d<Type>::vgl_plane_3d(vgl_homg_plane_3d<Type> const& p)
  : a_(p.a()), b_(p.b()), c_(p.c()), d_(p.d()) {assert (a_||b_||c_);}

//: Construct from three points
template <class Type>
vgl_plane_3d<Type>::vgl_plane_3d (vgl_point_3d<Type> const& p1,
                                  vgl_point_3d<Type> const& p2,
                                  vgl_point_3d<Type> const& p3)
: a_(p2.y()*p3.z()-p2.z()*p3.y()
    +p3.y()*p1.z()-p3.z()*p1.y()
    +p1.y()*p2.z()-p1.z()*p2.y())
, b_(p2.z()*p3.x()-p2.x()*p3.z()
    +p3.z()*p1.x()-p3.x()*p1.z()
    +p1.z()*p2.x()-p1.x()*p2.z())
, c_(p2.x()*p3.y()-p2.y()*p3.x()
    +p3.x()*p1.y()-p3.y()*p1.x()
    +p1.x()*p2.y()-p1.y()*p2.x())
, d_(p1.x()*(p2.z()*p3.y()-p2.y()*p3.z())
    +p2.x()*(p2.z()*p3.y()-p2.y()*p3.z())
    +p3.x()*(p2.z()*p3.y()-p2.y()*p3.z()))
{
  assert(a_||b_||c_||d_); // points should not be collinear or coinciding
}

//: Construct from normal and a point
template <class Type>
vgl_plane_3d<Type>::vgl_plane_3d(vgl_vector_3d<Type> const& n,
                                 vgl_point_3d<Type> const& p)
 : a_(n.x()), b_(n.y()), c_(n.z()), d_(n.x()*p.x()+n.y()*p.y()+n.z()*p.z()) {}

template <class Type>
bool vgl_plane_3d<Type>::operator==(vgl_plane_3d<Type> const& p) const
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
vcl_ostream& operator<<(vcl_ostream& s, const vgl_plane_3d<Type>& p) {
  return s << " <vgl_plane_3d "
           << p.a() << " x + "
           << p.b() << " y + "
           << p.c() << " z + "
           << p.d() << " = 0 >";
}

template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vgl_plane_3d<Type>& p) {
  Type a, b, c, d;
  s >> a >> b >> c >> d;
  p.set(a,b,c,d);
  return s;
}

#undef VGL_PLANE_3D_INSTANTIATE
#define VGL_PLANE_3D_INSTANTIATE(T) \
template class vgl_plane_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_plane_3d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_plane_3d<T >&)

#endif // vgl_plane_3d_txx_
