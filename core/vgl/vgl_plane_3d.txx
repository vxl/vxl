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
#include <vgl/vgl_intersection.h>
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

//: Return true if p is on the plane
template <class T>
bool vgl_plane_3d<T>::contains(vgl_point_3d<T> const& p, T tol)
{
  //to maintain a consistent distance metric the plane should be normalized
  vgl_vector_3d<T> n(a_, b_, c_), pv(p.x(), p.y(), p.z());
  T len = static_cast<T>(length(n));
  T dist = dot_product(n,pv);
  dist = (dist + d_)/len;
  return (dist<-tol || dist>tol);
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
vcl_istream& operator>>(vcl_istream& is, vgl_plane_3d<T>& p)
{
  if (! is.good()) return is; // (TODO: should throw an exception)
  bool paren = false;
  bool formatted = false;
  T a, b, c, d;
  is >> vcl_ws; // jump over any leading whitespace
  if (is.eof()) return is; // nothing to be set because of EOF (TODO: should throw an exception)
  if (is.peek() == '(') { is.ignore(); paren=true; }
  is >> vcl_ws >> a >> vcl_ws;
  if (is.eof()) return is;
  if (is.peek() == ',') is.ignore();
  else if (is.peek() == 'x') { is.ignore(); formatted=true; }
  is >> vcl_ws >> b >> vcl_ws;
  if (is.eof()) return is;
  if (formatted) {
    if (is.eof()) return is;
    if (is.peek() == 'y') is.ignore();
    else                  return is; // formatted input incorrect (TODO: throw an exception)
  }
  else if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> c >> vcl_ws;
  if (is.eof()) return is;
  if (formatted) {
    if (is.eof()) return is;
    if (is.peek() == 'z') is.ignore();
    else                  return is; // formatted input incorrect (TODO: throw an exception)
  }
  else if (is.peek() == ',') is.ignore();
  is >> vcl_ws >> d >> vcl_ws;
  if (paren) {
    if (is.eof()) return is;
    if (is.peek() == ')') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
  }
  if (formatted) {
    if (is.eof()) return is;
    if (is.peek() == '=') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
    is >> vcl_ws;
    if (is.peek() == '0') is.ignore();
    else                  return is; // closing parenthesis is missing (TODO: throw an exception)
  }
  p.set(a,b,c,d);
  return is;
}

#undef VGL_PLANE_3D_INSTANTIATE
#define VGL_PLANE_3D_INSTANTIATE(T) \
template class vgl_plane_3d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_plane_3d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_plane_3d<T >&)

#endif // vgl_plane_3d_txx_
