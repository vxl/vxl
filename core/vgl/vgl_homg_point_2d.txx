// This is vxl/vgl/vgl_homg_point_2d.txx
#ifndef vgl_homg_point_2d_txx_
#define vgl_homg_point_2d_txx_

//:
// \file
// \brief Represents a homogeneous 2D point.
// \author Don Hamilton, Peter Tu
// \date   Feb 15 2000

#include "vgl_homg_point_2d.h"
#include <vgl/vgl_homg_line_2d.h>
#include <vcl_iostream.h>

// Note that the given lines must be distinct!
template <class Type>
vgl_homg_point_2d<Type>::vgl_homg_point_2d (vgl_homg_line_2d<Type> const& l1,
                                            vgl_homg_line_2d<Type> const& l2)
{
  set(l1.b()*l2.c()-l1.c()*l2.b(),
      l1.c()*l2.a()-l1.a()*l2.c(),
      l1.a()*l2.b()-l1.b()*l2.a());
}

template <class Type>
bool vgl_homg_point_2d<Type>::operator==(vgl_homg_point_2d<Type> const& other) const
{
  return (this==&other) ||
         (   this->x()*other.w() == this->w()*other.x()
          && this->y()*other.w() == this->w()*other.y());
}

template <class Type>
vcl_ostream&  operator<<(vcl_ostream& s, const vgl_homg_point_2d<Type>& p) {
  return s << " <vgl_homg_point_2d ("
           << p.x() << "," << p.y() << "," << p.w() << ") >";
}

template <class Type>
vcl_istream&  operator>>(vcl_istream& is,  vgl_homg_point_2d<Type>& p) {
  Type x, y, w;
  is >> x >> y >> w;
  p.set(x,y,w);
  return is;
}

#define VGL_HOMG_POINT_2D_INSTANTIATE(T) \
template class vgl_homg_point_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_point_2d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_point_2d<T >&)

#endif // vgl_homg_point_2d_txx_
