// This is core/vgl/vgl_homg_point_1d.txx
#ifndef vgl_homg_point_1d_txx_
#define vgl_homg_point_1d_txx_

#include "vgl_homg_point_1d.h"
#include <vcl_iostream.h>

template <class Type>
vcl_ostream& operator<<(vcl_ostream& s, vgl_homg_point_1d<Type> const& p)
{
  return s << "<vgl_homg_point_1d (" << p.x() << ',' << p.w() << ") > ";
}

template <class Type>
vcl_istream& operator>>(vcl_istream& s, vgl_homg_point_1d<Type>& p)
{
  Type x, w; s >> x >> w; p.set(x,w); return s;
}

#undef VGL_HOMG_POINT_1D_INSTANTIATE
#define VGL_HOMG_POINT_1D_INSTANTIATE(T) \
template class vgl_homg_point_1d<T >; \
template vcl_ostream& operator<<(vcl_ostream&, vgl_homg_point_1d<T >const&); \
template vcl_istream& operator>>(vcl_istream&, vgl_homg_point_1d<T >&)

#endif // vgl_homg_point_1d_txx_
