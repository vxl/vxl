
#include "vgl_line_segment_3d.h"

#include <vcl/vcl_iostream.h>

template <class Type> 
vgl_line_segment_3d<Type>::vgl_line_segment_3d(vgl_line_segment_3d<Type> const& that)
{
}

template <class Type> 
vgl_line_segment_3d<Type>::vgl_line_segment_3d(vgl_point_3d<Type> const & point1, vgl_point_3d<Type> const & point2):
  point1_(point1),
  point2_(point2)
{
}

template <class Type> 
vgl_line_segment_3d<Type>::~vgl_line_segment_3d()
{
}

// stream operators 
template <class Type> 
ostream& operator<<(ostream& s, vgl_line_segment_3d<Type> const & p)
{
  return s << p.get_point1() << " " << p.get_point2() << "\n";
}

template <class Type>
istream& operator>>(istream& is, vgl_line_segment_3d<Type>& p)
{
  return is >> p.get_point1() >> p.get_point2();
}

#define VGL_LINE_SEGMENT_3D_INSTANTIATE(Type) \
template class vgl_line_segment_3d<Type >;\
template istream& operator>>(istream&, vgl_line_segment_3d<Type >&);\
template ostream& operator<<(ostream&, vgl_line_segment_3d<Type > const&);
