// This is core/vgl/vgl_line_segment_3d.hxx
#ifndef vgl_line_segment_3d_hxx_
#define vgl_line_segment_3d_hxx_

#include <iostream>
#include "vgl_line_segment_3d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// stream operators
template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_line_segment_3d<Type> const & p)
{
  return s << "<vgl_line_segment_3d " << p.point1() << " to " << p.point2() << " >";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_line_segment_3d<Type>& p)
{
  vgl_point_3d<Type> p1, p2;
  s >> p1 >> p2;
  p.set(p1, p2);
  return s;
}

#undef VGL_LINE_SEGMENT_3D_INSTANTIATE
#define VGL_LINE_SEGMENT_3D_INSTANTIATE(Type) \
template class vgl_line_segment_3d<Type >;\
template std::istream& operator>>(std::istream&, vgl_line_segment_3d<Type >&);\
template std::ostream& operator<<(std::ostream&, vgl_line_segment_3d<Type > const&)

#endif // vgl_line_segment_3d_hxx_
