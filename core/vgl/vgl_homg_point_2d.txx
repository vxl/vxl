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

// Note that the given lines must be distinct!
template <class Type>
vgl_homg_point_2d<Type>::vgl_homg_point_2d (vgl_homg_line_2d<Type> const& l1,
                                            vgl_homg_line_2d<Type> const& l2)
{
  set(l1.b()*l2.c()-l1.c()*l2.b(),
      l1.c()*l2.a()-l1.a()*l2.c(),
      l1.a()*l2.b()-l1.b()*l2.a());
}

#define VGL_HOMG_POINT_2D_INSTANTIATE(T) \
template class vgl_homg_point_2d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_point_2d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_point_2d<T >&)

#endif // vgl_homg_point_2d_txx_
