// This is vxl/vgl/vgl_line_segment_3d.txx
#ifndef vgl_line_segment_3d_txx_
#define vgl_line_segment_3d_txx_

#include "vgl_line_segment_3d.h"

#include <vcl_iostream.h>

// stream operators
template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_line_segment_3d<Type> const & p)
{
  return s << "<vgl_line_segment_3d " << p.point1() << " to " << p.point2() << " >";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_line_segment_3d<Type>& p)
{
  vgl_point_3d<Type> p1, p2;
  s >> p1 >> p2;
  p.set(p1, p2);
  return s;
}

#undef VGL_LINE_SEGMENT_3D_INSTANTIATE
#define VGL_LINE_SEGMENT_3D_INSTANTIATE(Type) \
template class vgl_line_segment_3d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_line_segment_3d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_segment_3d<Type > const&)

#endif // vgl_line_segment_3d_txx_
