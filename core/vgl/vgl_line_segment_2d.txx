// This is core/vgl/vgl_line_segment_2d.txx
#ifndef vgl_line_segment_2d_txx_
#define vgl_line_segment_2d_txx_

#include "vgl_line_segment_2d.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>
// stream operators
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_line_segment_2d<Type> const & p)
{
  return s << "<vgl_line_segment_2d " << p.point1() << " to " << p.point2() << " >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_line_segment_2d<Type>& p)
{
  vgl_point_2d<Type> p1, p2;
  s >> p1 >> p2;
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
vgl_vector_2d<double>  vgl_line_segment_2d<Type>::direction() const
{
  vgl_vector_2d<double> v(point2_.x()-point1_.x(),point2_.y()-point1_.y());
  return normalized(v);
}

template <class Type>
vgl_vector_2d<double>  vgl_line_segment_2d<Type>::normal() const
{
  vgl_vector_2d<double> v(point1_.y()-point2_.y(),point2_.x()-point1_.x());
  return normalized(v);
}

template <class Type>
double vgl_line_segment_2d<Type>::slope_degrees() const
{
  static const double deg_per_rad = 45.0/vcl_atan2(1.0,1.0);
  double dy = point2_.y()-point1_.y();
  double dx = point2_.x()-point1_.x();
  // do special cases separately, to avoid rounding errors:
  if (dx == 0) return dy<0 ? -90.0 : 90.0;
  if (dy == 0) return dx<0 ? 180.0 : 0.0;
  if (dy == dx) return dy<0 ? -135.0 : 45.0;
  if (dy+dx == 0) return dy<0 ? -45.0 : 135.0;
  // general case:
  return deg_per_rad * vcl_atan2(dy,dx);
}

template <class Type>
double vgl_line_segment_2d<Type>::slope_radians() const
{
  double dy = point2_.y()-point1_.y();
  double dx = point2_.x()-point1_.x();
  return vcl_atan2(dy,dx);
}

#undef VGL_LINE_SEGMENT_2D_INSTANTIATE
#define VGL_LINE_SEGMENT_2D_INSTANTIATE(Type) \
template class vgl_line_segment_2d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_line_segment_2d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_segment_2d<Type > const&)

#endif // vgl_line_segment_2d_txx_
