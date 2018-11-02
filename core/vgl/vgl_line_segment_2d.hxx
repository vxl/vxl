// This is core/vgl/vgl_line_segment_2d.hxx
#ifndef vgl_line_segment_2d_hxx_
#define vgl_line_segment_2d_hxx_

#include <iostream>
#include <cmath>
#include <string>
#include "vgl_line_segment_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
// stream operators
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_line_segment_2d<Type> const & p)
{
  return s << "<vgl_line_segment_2d " << p.point1() << " to " << p.point2() << " >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_line_segment_2d<Type>& p)
{
  std::string temp;
  s >> std::skipws >> std::ws;
  char c = s.peek();
  if(c=='<')
    s >> temp;
  vgl_point_2d<Type> p1, p2;
  s >> p1>>std::ws;
  c = s.peek();
  if(c=='t')
  s >> temp;
  s >> std::ws >> p2 >> temp;
  p.set(p1, p2);
  return s;
}

template <class Type>
Type vgl_line_segment_2d<Type>::a() const
{
  return point1_.y()-point2_.y();
}

template <class Type>
Type vgl_line_segment_2d<Type>::b() const
{
  return point2_.x()-point1_.x();
}

template <class Type>
Type vgl_line_segment_2d<Type>::c() const
{
  return point1_.x()*point2_.y()-point2_.x()*point1_.y();
}

template <class Type>
vgl_vector_2d<Type>  vgl_line_segment_2d<Type>::direction() const
{
  vgl_vector_2d<Type> v(point2_.x()-point1_.x(),point2_.y()-point1_.y());
  return normalized(v);
}

template <class Type>
vgl_vector_2d<Type>  vgl_line_segment_2d<Type>::normal() const
{
  vgl_vector_2d<Type> v(point1_.y()-point2_.y(),point2_.x()-point1_.x());
  return normalized(v);
}

template <class Type>
double vgl_line_segment_2d<Type>::slope_degrees() const
{
  static const double deg_per_rad = 45.0/std::atan2(1.0,1.0);
  double dy = point2_.y()-point1_.y();
  double dx = point2_.x()-point1_.x();
  // do special cases separately, to avoid rounding errors:
  if (dx == 0) return dy<0 ? -90.0 : 90.0;
  if (dy == 0) return dx<0 ? 180.0 : 0.0;
  if (dy == dx) return dy<0 ? -135.0 : 45.0;
  if (dy+dx == 0) return dy<0 ? -45.0 : 135.0;
  // general case:
  return deg_per_rad * std::atan2(dy,dx);
}

template <class Type>
double vgl_line_segment_2d<Type>::slope_radians() const
{
  double dy = point2_.y()-point1_.y();
  double dx = point2_.x()-point1_.x();
  return std::atan2(dy,dx);
}

#undef VGL_LINE_SEGMENT_2D_INSTANTIATE
#define VGL_LINE_SEGMENT_2D_INSTANTIATE(Type) \
template class vgl_line_segment_2d<Type >;\
template std::istream& operator>>(std::istream&, vgl_line_segment_2d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_line_segment_2d<Type > const&)

#endif // vgl_line_segment_2d_hxx_
