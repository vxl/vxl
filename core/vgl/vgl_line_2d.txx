// This is vxl/vgl/vgl_line_2d.txx
#ifndef vgl_line_2d_txx_
#define vgl_line_2d_txx_
//:
// \file

#include "vgl_line_2d.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>

//: line through two given points
template <class Type>
vgl_line_2d<Type>::vgl_line_2d (vgl_point_2d<Type> const& p1, vgl_point_2d<Type> const& p2)
: a_ ( p1.y() - p2.y() )
, b_ ( p2.x() - p1.x() )
, c_ ( p1.x() * p2.y() - p1.y() * p2.x() )
{
  assert(a_||b_); // two points were distinct
}

template <class Type>
vgl_line_2d<Type>::vgl_line_2d (vgl_homg_line_2d<Type> const& l)
 : a_(l.a()) , b_(l.b()) , c_(l.c())
{
  assert(c_);
}

//: Get two points on the line.
// These two points are normally the intersections
// with the Y axis and X axis, respectively.  When the line is parallel to one
// of these, the point with \a y=1 or \a x=1, resp. are taken.
// When the line goes through the origin, the second point is \a (b,-a).
template <class Type>
void vgl_line_2d<Type>::get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2)
{
  if (b() == 0)       p1.set(-c()/a(), 1);
  else                p1.set(0, -c()/b());
  if (a() == 0)       p2.set(1, -c()/b());
  else if ( c() == 0) p2.set(b(), -a());
  else                p2.set(-c()/a(), 0);
}

//: Write line description to stream: "<vgl_line_2d ax+by+c>"
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, vgl_line_2d<Type> const& line) {
  return s << " <vgl_line_2d " << line.a() << " x + " << line.b()
           << " y + " << line.c() << " = 0>";
}

//: Read in three line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vgl_line_2d<Type>& line) {
  Type a,b,c;
  s >> a >> b >> c;
  line.set(a,b,c);
  return s;
}

#undef VGL_LINE_2D_INSTANTIATE
#define VGL_LINE_2D_INSTANTIATE(T) \
template class vgl_line_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_2d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_line_2d<T >&)

#endif // vgl_line_2d_txx_
