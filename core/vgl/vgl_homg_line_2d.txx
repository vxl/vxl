// This is core/vgl/vgl_homg_line_2d.txx
#ifndef vgl_homg_line_2d_txx_
#define vgl_homg_line_2d_txx_
//:
// \file

#include "vgl_homg_line_2d.h"
#include <vcl_cmath.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_line_2d.h>

template <class Type>
vgl_homg_line_2d<Type>::vgl_homg_line_2d (vgl_line_2d<Type> const& l)
  : a_(l.a()) , b_(l.b()) , c_(l.c())
{
}

//: get two points on the line.
//  These two points are normally the intersections with the Y axis and X axis,
//  respectively.  When the line is parallel to one of these, the point with
//  \a y/w=1 or \a x/w=1, resp. are taken.  When the line goes through the origin,
//  the second point is (b, -a, 1).  Finally, when the line is the line at
//  infinity, the returned points are (1,0,0) and (0,1,0).
//
//  Thus, whenever possible, the returned points are not at infinity.
//
template <class Type>
void vgl_homg_line_2d<Type>::get_two_points(vgl_homg_point_2d<Type> &p1, vgl_homg_point_2d<Type> &p2) const
{
  if (      b() == 0) p1.set(-c(), a(), a());
  else                p1.set(0, -c(), b());
  if (      a() == 0) p2.set(b(), -c(), b());
  else if ( c() == 0) p2.set(b(), -a(), 1);
  else                p2.set(-c(), 0, a());
}

template <class Type>
vgl_homg_line_2d<Type>::vgl_homg_line_2d (vgl_homg_point_2d<Type> const& p1,
                                          vgl_homg_point_2d<Type> const& p2)
{
  set(p1.y()*p2.w()-p1.w()*p2.y(),
      p1.w()*p2.x()-p1.x()*p2.w(),
      p1.x()*p2.y()-p1.y()*p2.x());
  assert(a_||b_||c_); // given points should be different
}

#define vp(os,v,s) { os<<' '; if ((v)>0) os<<'+';\
                     if ((v)&&!s[0]) os<<(v); else { \
                       if ((v)==-1) os<<'-';\
                       else if ((v)!=0&&(v)!=1) os<<(v);\
                       if ((v)!=0) os<<' '<<s; } }

//: Print line equation to stream
template <class Type>
vcl_ostream&  operator<<(vcl_ostream& os, vgl_homg_line_2d<Type>const& l)
{
  os << "<vgl_homg_line_2d"; vp(os,l.a(),"x"); vp(os,l.b(),"y"); vp(os,l.c(),"w");
  return os << " = 0 >";
}

#undef vp

//: Load in line parameters from stream
template <class Type>
vcl_istream&  operator>>(vcl_istream& is, vgl_homg_line_2d<Type>& p)
{
  Type a,b,c;
  is >> a >> b >> c;
  p.set(a,b,c);
  return is;
}

template <class Type>
void vgl_homg_line_2d<Type>::normalize()
{
  double sum = a_*a_ + b_*b_;
  double den = vcl_sqrt(sum);
  if (den<1.0e-8)//don't normalize ideal line
    return;
  double an= (double)a()/den;
  double bn= (double)b()/den;
  double cn= (double)c()/den;
  //standardize so that a is positive unless a is smaller than b, then
  //standardize the sign of b
  if (vcl_fabs(an)>vcl_fabs(bn))
    if (an>0)
    {
      a_ = (Type)an;
      b_ = (Type)bn;
      c_ = (Type)cn;
    }
    else
    {
      a_ = -(Type)an;
      b_ = -(Type)bn;
      c_ = -(Type)cn;
    }
  else
    if (bn>0)
    {
      a_ = (Type)an;
      b_ = (Type)bn;
      c_ = (Type)cn;
    }
    else
    {
      a_ = -(Type)an;
      b_ = -(Type)bn;
      c_ = -(Type)cn;
    }

  return;
}

#undef VGL_HOMG_LINE_2D_INSTANTIATE
#define VGL_HOMG_LINE_2D_INSTANTIATE(T) \
template class vgl_homg_line_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_line_2d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_line_2d<T >&)

#endif // vgl_homg_line_2d_txx_
