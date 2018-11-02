// This is core/vgl/vgl_homg_point_1d.hxx
#ifndef vgl_homg_point_1d_hxx_
#define vgl_homg_point_1d_hxx_

#include <iostream>
#include "vgl_homg_point_1d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class Type>
std::ostream& operator<<(std::ostream& s, vgl_homg_point_1d<Type> const& p)
{
  return s << "<vgl_homg_point_1d (" << p.x() << ',' << p.w() << ") > ";
}

template <class Type>
std::istream& operator>>(std::istream& s, vgl_homg_point_1d<Type>& p)
{
  Type x, w; s >> x >> w; p.set(x,w); return s;
}

#undef VGL_HOMG_POINT_1D_INSTANTIATE
#define VGL_HOMG_POINT_1D_INSTANTIATE(T) \
template class vgl_homg_point_1d<T >; \
template std::ostream& operator<<(std::ostream&, vgl_homg_point_1d<T >const&); \
template std::istream& operator>>(std::istream&, vgl_homg_point_1d<T >&)

#endif // vgl_homg_point_1d_hxx_
