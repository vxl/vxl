// This is vxl/vgl/vgl_homg_line_2d.txx
#ifndef vgl_homg_line_2d_txx_
#define vgl_homg_line_2d_txx_

#include <vcl_iostream.h>
#include "vgl_homg_line_2d.h"
#include <vgl/vgl_homg_point_2d.h>

#if 0 // TODO
//: return the direction of the line
Type get_direction() const
{
  Type direction[3];
  direction[0]=this->a();
  direction[1]=this->b();
  direction[2]=0;
  return direction;
}

vcl_vector<Type> get_normal() const;
{
  vcl_vector<Type,3> normal;
  normal[0]= - this->b();
  normal[1]=this->a();
  normal[2]=0;
  return normal;
}
#endif

//: get two points on the line.  These two points are normally the intersections
// with the Y axis and X axis, respectively.  When the line is parallel to one
// of these, the point with y=1 or x=1, resp. are taken.  When the line goes
// through the origin, the second point is (b, -a, 1).  Finally, when the line
// is the line at infinity, the returned points are (1,0,0) and (0,1,0).
// Thus, whenever possible, the returned points are not at infinity.
template <class Type>
void vgl_homg_line_2d<Type>::get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2)
{
  if (this->b() == 0) p1.set(-this->c(), this->a(), this->a());
  else                p1.set(0, -this->c(), this->b());
  if (this->a() == 0) p2.set(this->b(), -this->c(), this->b());
  else if ( c() == 0) p2.set(this->b(), -this->a(), 1);
  else                p2.set(-this->c(), 0, this->a());
}

// Note that the given points must be distinct!
template <class Type>
vgl_homg_line_2d<Type>::vgl_homg_line_2d (vgl_homg_point_2d<Type> const& l1,
                                          vgl_homg_point_2d<Type> const& l2)
{
  set(l1.y()*l2.w()-l1.w()*l2.y(),
      l1.w()*l2.x()-l1.x()*l2.w(),
      l1.x()*l2.y()-l1.y()*l2.x());
}

template <class Type>
bool vgl_homg_line_2d<Type>::operator==(vgl_homg_line_2d<Type> const& other) const
{
  return (this==&other) ||
         (   this->a()*other.c() == this->c()*other.a()
          && this->b()*other.c() == this->c()*other.b());
}

//: Print line equation to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_line_2d<Type>& p) {
  return s << " <vgl_homg_line_2d "
           << p.a() << " x + " << p.b() << " y + "
           << p.c() << " z = 0>";
}

//: Load in line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_homg_line_2d<Type>& p) {
  Type a,b,c;
  is >> a >> b >> c;
  p.set(a,b,c);
  return is;
}

#define VGL_HOMG_LINE_2D_INSTANTIATE(T) \
template class vgl_homg_line_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, const vgl_homg_line_2d<T >&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_line_2d<T >&)

#endif // vgl_homg_line_2d_txx_
