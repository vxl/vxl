// This is core/vgl/vgl_1d_basis.hxx
#ifndef vgl_1d_basis_hxx_
#define vgl_1d_basis_hxx_

#include <iostream>
#include "vgl_1d_basis.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
vgl_1d_basis<T>::vgl_1d_basis(T const& o, T const& u, T const& i)
  : origin_(o), unity_(u), inf_pt_(i), affine_(false)
{
  assert(collinear(o,i,u) && o!=i && o!=u && i!=u);
}

template <class T>
vgl_1d_basis<T>::vgl_1d_basis(T const& o, T const& u)
  : origin_(o), unity_(u), affine_(true)
{
  assert(o!=u && !is_ideal(o) && !is_ideal(u));
}

template <class T>
vgl_homg_point_1d<double> vgl_1d_basis<T>::project(T const& p)
{
  if (affine_) // In this case, do not use the uninitialised inf_pt_
  {
    double d = ratio(origin_,unity_,p);
    return {d,1};
  }
  else // !affine_
  {
    if (p == inf_pt_) return {1,0};
    double d = cross_ratio(inf_pt_,origin_,unity_,p);
    return {d,1};
  }
}

template <class T>
std::ostream& operator<<(std::ostream& s, vgl_1d_basis<T> const& b)
{
  s << "<vgl_1d_basis "<< b.origin() << ' ' << b.unity();
  if (!b.affine()) s << ' ' << b.inf_pt();
  s <<  " > ";
  return s;
}

#undef VGL_1D_BASIS_INSTANTIATE
#define VGL_1D_BASIS_INSTANTIATE(T) \
template class vgl_1d_basis<T >;\
template std::ostream& operator<<(std::ostream&, vgl_1d_basis<T > const&)

#endif // vgl_1d_basis_hxx_
