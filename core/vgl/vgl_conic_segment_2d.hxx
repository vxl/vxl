// This is core/vgl/vgl_conic_segment_2d.hxx
#ifndef vgl_conic_segment_2d_hxx_
#define vgl_conic_segment_2d_hxx_

#include <iostream>
#include "vgl_conic_segment_2d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// stream operators
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_conic_segment_2d<Type> const & p)
{
  return s << "<vgl_conic_segment_2d " << p.point1() << " to " << p.point2() << "with " << p.conic()
           << (p.is_clockwise() ? ", clockwise" : ", counterclockwise") << " >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_conic_segment_2d<Type>& p)
{
  vgl_point_2d<Type> p1, p2;
  s >> p1 >> p2;
  vgl_conic<Type> conic;
  s >> conic;
  p.set(p1, p2, conic, true);
  return s;
}

#undef VGL_CONIC_SEGMENT_2D_INSTANTIATE
#define VGL_CONIC_SEGMENT_2D_INSTANTIATE(Type) \
template class vgl_conic_segment_2d<Type >;\
template std::istream& operator>>(std::istream&, vgl_conic_segment_2d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_conic_segment_2d<Type > const&)

#endif // vgl_conic_segment_2d_hxx_
