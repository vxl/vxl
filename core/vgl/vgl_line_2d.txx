// This is core/vgl/vgl_line_2d.txx
#ifndef vgl_line_2d_txx_
#define vgl_line_2d_txx_
//:
// \file

#include "vgl_line_2d.h"
#include <vcl_cmath.h>     // vcl_sqrt()
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

//: line defined by one point and one vector
template <class Type>
vgl_line_2d<Type>::vgl_line_2d (vgl_point_2d<Type> const& p, vgl_vector_2d<Type> const& v)
: a_ ( -v.y() )
, b_ ( v.x() )
, c_ ( -a_*p.x() - b_*p.y() )
{
}
template <class Type>
vgl_line_2d<Type>::vgl_line_2d (vgl_homg_line_2d<Type> const& l)
 : a_(l.a()) , b_(l.b()) , c_(l.c())
{
  //JLM I see no reason to prohibit lines through the origin
  //  assert(c_);
}

//: Get two points on the line.
// These two points are normally the intersections
// with the Y axis and X axis, respectively.  When the line is parallel to one
// of these, the point with \a y=1 or \a x=1, resp. are taken.
// When the line goes through the origin, the second point is \a (b,-a).
template <class Type>
void vgl_line_2d<Type>::get_two_points(vgl_point_2d<Type> &p1, vgl_point_2d<Type> &p2) const
{
  if (b() == 0)       p1.set(-c()/a(), 1);
  else                p1.set(0, -c()/b());
  if (a() == 0)       p2.set(1, -c()/b());
  else if ( c() == 0) p2.set(b(), -a());
  else                p2.set(-c()/a(), 0);
}

template <class Type>
double vgl_line_2d<Type>::slope_degrees() const
{
  static const double deg_per_rad = 45.0/vcl_atan2(1.0,1.0);
  // do special cases separately, to avoid rounding errors:
  if (a() == 0) return b()<0 ? 0.0 : 180.0;
  if (b() == 0) return a()<0 ? -90.0 : 90.0;
  if (a() == b()) return a()<0 ? -45.0 : 135.0;
  if (a()+b() == 0) return a()<0 ? -135.0 : 45.0;
  // general case:
  return deg_per_rad * vcl_atan2(double(a()),-double(b()));
}

template <class Type>
double vgl_line_2d<Type>::slope_radians() const
{
  return vcl_atan2(double(a()),-double(b()));
}

template <class Type>
bool vgl_line_2d<Type>::normalize()
{
  double mag = a_*a_ + b_*b_;
  if (mag==1.0) return true;
  if (mag==0.0) return false;
  mag = 1.0/vcl_sqrt(mag);
  a_ = Type(a_*mag);
  b_ = Type(b_*mag);
  c_ = Type(c_*mag);
  mag = a_*a_ + b_*b_;
  // return false when normalisation did not succeed, e.g. when Type == int:
  return mag>0.99 && mag<1.01;
}

#define vp(os,v,s) { os<<' '; if ((v)>0) os<<'+'; if ((v)&&!s[0]) os<<(v); else { \
                     if ((v)==-1) os<<'-';\
                     else if ((v)!=0&&(v)!=1) os<<(v);\
                     if ((v)!=0) os<<' '<<s; } }

//: Write line description to stream: "<vgl_line_2d ax+by+c=0>"
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& os, vgl_line_2d<Type> const& l)
{
  os << "<vgl_line_2d"; vp(os,l.a(),"x"); vp(os,l.b(),"y"); vp(os,l.c(),"");
  return os << " = 0 >";
}

#undef vp

//: Read in three line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& s, vgl_line_2d<Type>& line)
{
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
