// This is core/vgl/vgl_plane_3d.txx
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
template <class T>
vgl_plane_3d<T>::vgl_plane_3d(vgl_homg_plane_3d<T> const& p)
  : a_(p.a()), b_(p.b()), c_(p.c()), d_(p.d()) {assert (a_||b_||c_);}

//: Construct from three points
template <class T>
vgl_plane_3d<T>::vgl_plane_3d (vgl_point_3d<T> const& p1,
                               vgl_point_3d<T> const& p2,
                               vgl_point_3d<T> const& p3)
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
    +p2.x()*(p3.z()*p1.y()-p3.y()*p1.z())
    +p3.x()*(p1.z()*p2.y()-p1.y()*p2.z()))
{
  assert(a_||b_||c_); // points should not be collinear or coinciding
}

//: Construct from normal and a point
template <class T>
vgl_plane_3d<T>::vgl_plane_3d(vgl_vector_3d<T> const& n,
                              vgl_point_3d<T> const& p)
 : a_(n.x()), b_(n.y()), c_(n.z()), d_(-(n.x()*p.x()+n.y()*p.y()+n.z()*p.z()))
{
  assert(a_||b_||c_); // normal vector should not be the null vector
}

template <class T>
bool vgl_plane_3d<T>::operator==(vgl_plane_3d<T> const& p) const
{
  return (this==&p) ||
         (   (a()*p.b()==p.a()*b())
          && (a()*p.c()==p.a()*c())
          && (a()*p.d()==p.a()*d())
          && (b()*p.c()==p.b()*c())
          && (b()*p.d()==p.b()*d())
          && (c()*p.d()==p.c()*d()) );
}

#define vp(os,v,s) { os<<' '; if ((v)>0) os<<'+'; if ((v)&&!s[0]) os<<(v); else { \
                     if ((v)==-1) os<<'-';\
                     else if ((v)!=0&&(v)!=1) os<<(v);\
                     if ((v)!=0) os<<' '<<s; } }

template <class T>
vcl_ostream& operator<<(vcl_ostream& os, const vgl_plane_3d<T>& p)
{
  os << "<vgl_plane_3d"; vp(os,p.a(),"x"); vp(os,p.b(),"y"); vp(os,p.c(),"z");
  vp(os,p.d(),""); return os << " = 0 >";
}

#undef vp

template <class T>
vcl_istream& operator>>(vcl_istream& s, vgl_plane_3d<T>& p)
{
  T a, b, c, d;
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
