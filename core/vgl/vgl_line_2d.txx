// This is vxl/vgl/vgl_line_2d.txx
#ifndef vgl_line_2d_txx_
#define vgl_line_2d_txx_

#include <vcl_iostream.h>
#include "vgl_line_2d.h"
#include <vgl/vgl_point_2d.h>

//: get two points on the line.  These two points are normally the intersections
// with the Y axis and X axis, respectively.  When the line is parallel to one
// of these, the point with y=1 or x=1, resp. are taken.  When the line goes
// through the origin, the second point is (b, -a).
template <class Type>
void vgl_line_2d<Type>::get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2)
{
  if (b() == 0)       p1.set(-c()/a(), 1);
  else                p1.set(0, -c()/b());
  if (a() == 0)       p2.set(1, -c()/b());
  else if ( c() == 0) p2.set(b(), -a());
  else                p2.set(-c()/a(), 0);
}

template <class Type>
bool vgl_line_2d<Type>::operator==(vgl_line_2d<Type> const& other) const
{
  return (this==&other) ||
         (   this->a()*other.c() == this->c()*other.a()
          && this->b()*other.c() == this->c()*other.b());
}

//: Write line description to stream: "<vgl_line_2d ax+by+c>"
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_line_2d<Type>& p) {
  return s << " <vgl_line_2d " << p.a() << " x + " << p.b()
           << " y + " << p.c() << " = 0>";
}

//: Read in three line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_line_2d<Type>& p) {
  Type a,b,c;
  is >> a >> b >> c;
  p.set(a,b,c);
  return is;
}


#define VGL_LINE_2D_INSTANTIATE(T) \
template class vgl_line_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_line_2d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_line_2d<T >&)

#endif // vgl_line_2d_txx_
