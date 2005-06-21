// This is core/vgl/vgl_conic_segment_2d.txx
#ifndef vgl_conic_segment_2d_txx_
#define vgl_conic_segment_2d_txx_

#include "vgl_conic_segment_2d.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>
// stream operators
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_conic_segment_2d<Type> const & p)
{
  return s << "<vgl_conic_segment_2d " << p.point1() << " to " << p.point2() << "with " << p.conic() << " >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_conic_segment_2d<Type>& p)
{
  vgl_point_2d<Type> p1, p2;
  s >> p1 >> p2;
  vgl_conic<Type> conic;
  s >> conic;
  p.set(p1, p2, conic);
  return s;
}

#undef VGL_CONIC_SEGMENT_2D_INSTANTIATE
#define VGL_CONIC_SEGMENT_2D_INSTANTIATE(Type) \
template class vgl_conic_segment_2d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_conic_segment_2d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_conic_segment_2d<Type > const&)

#endif // vgl_conic_segment_2d_txx_
