// vgl/vgl_line_segment_2d.txx

#include "vgl_line_segment_2d.h"

#include <vcl_iostream.h>

template <class Type> 
vgl_line_segment_2d<Type>::vgl_line_segment_2d(vgl_line_segment_2d<Type> const& that)
{
}

template <class Type> 
vgl_line_segment_2d<Type>::vgl_line_segment_2d(vgl_point_2d<Type> const & point1, vgl_point_2d<Type> const & point2):
  point1_(point1),
  point2_(point2)
{
}

template <class Type> 
vgl_line_segment_2d<Type>::~vgl_line_segment_2d()
{
}

// stream operators 
template <class Type> 
vcl_ostream& operator<<(vcl_ostream& s, vgl_line_segment_2d<Type> const & p)
{
  return s << p.get_point1() << " " << p.get_point2() << "\n";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& is, vgl_line_segment_2d<Type>& p)
{
  return is >> p.get_point1() >> p.get_point2();
}

#define VGL_LINE_SEGMENT_2D_INSTANTIATE(Type) \
template class vgl_line_segment_2d<Type >;\
template vcl_istream& operator>>(vcl_istream&, vgl_line_segment_2d<Type >&);\
template vcl_ostream& operator<<(vcl_ostream&, vgl_line_segment_2d<Type > const&);
