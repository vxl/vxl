// This is vxl/vgl/vgl_point_2d.txx
#ifndef vgl_point_2d_txx_
#define vgl_point_2d_txx_

#include "vgl_point_2d.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_line_2d.h>

#include <vcl_iostream.h>
#include <vcl_cmath.h>

//: Construct from homogeneous point
template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_homg_point_2d<Type> const& p)
  : x_(p.x()/p.w()), y_(p.y()/p.w()) // could be infinite!
{
}

//: Construct from 2 lines (intersection).
template <class Type>
vgl_point_2d<Type>::vgl_point_2d(vgl_line_2d<Type> const& l1,
                                 vgl_line_2d<Type> const& l2)
{
  vgl_homg_line_2d<Type> h1(l1.a(), l1.b(), l1.c());
  vgl_homg_line_2d<Type> h2(l2.a(), l2.b(), l2.c());
  vgl_homg_point_2d<Type> p(h1, h2); // do homogeneous intersection
  set(p.x()/p.w(), p.y()/p.w()); // could be infinite!
}

//: Write "<vgl_point_3d x,y> " to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_point_2d<Type> const& p) {
  return s << "<vgl_point_2d "<< p.x() << "," << p.y() << ">";
}

//: Read x y from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_point_2d<Type>& p) {
  Type x, y; is >> x >> y; p.set(x,y); return is;
}

#undef VGL_POINT_2D_INSTANTIATE
#define VGL_POINT_2D_INSTANTIATE(T) \
template class vgl_point_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_point_2d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_point_2d<T >&)

#endif // vgl_point_2d_txx_
