// This is core/vgl/vgl_1d_basis.txx
#ifndef vgl_1d_basis_txx_
#define vgl_1d_basis_txx_

#include "vgl_1d_basis.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>

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
    return vgl_homg_point_1d<double>(d,1);
  }
  else // !affine_
  {
    if (p == inf_pt_) return vgl_homg_point_1d<double>(1,0);
    double d = cross_ratio(inf_pt_,origin_,unity_,p);
    return vgl_homg_point_1d<double>(d,1);
  }
}

template <class T>
vcl_ostream& operator<<(vcl_ostream& s, vgl_1d_basis<T> const& b)
{
  s << "<vgl_1d_basis "<< b.origin() << ' ' << b.unity();
  if (!b.affine()) s << ' ' << b.inf_pt();
  s <<  " > ";
  return s;
}

#undef VGL_1D_BASIS_INSTANTIATE
#define VGL_1D_BASIS_INSTANTIATE(T) \
template class vgl_1d_basis<T >;\
template vcl_ostream& operator<<(vcl_ostream&, vgl_1d_basis<T > const&)

#endif // vgl_1d_basis_txx_
