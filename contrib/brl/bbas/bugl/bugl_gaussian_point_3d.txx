#ifndef bugl_gaussian_point_3d_txx_
#define bugl_gaussian_point_3d_txx_

#include "bugl_gaussian_point_3d.h"
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector_fixed.h>

template<class T>
void bugl_gaussian_point_3d<T>::set_covariant_matrix(vnl_matrix_fixed<T, 3, 3> const& s)
{
  sigma_ = s;
  sigma_inv_ = vnl_inverse(s);
  det_ = vnl_det(s);
}

template<class T>
T bugl_gaussian_point_3d<T>::prob_at(vgl_point_3d<T> const& p) const
{
 if (!this->exists_)
  return 0;
  vnl_vector_fixed<T, 3> d(p.x() - this->x(), p.y() - this->y(), p.z() - this->z());

  const double pi = vnl_math::pi;
  return vcl_exp(-0.5*(dot_product(d, sigma_inv_*d)))/vcl_sqrt(8*pi)/pi/det_;
}

#if 0
template<class T>
bugl_gaussian_point_3d<T>& bugl_gaussian_point_3d<T>::operator=(bugl_gaussian_point_3d<T> const& p)
{
  vnl_matrix_fixed<T, 3, 3> t = p.get_covariant_matrix();
  set_covariant_matrix(t);
  set_point(p);
  return *this;
}
#endif // 0

//----------------------------------------------------------------------------
#undef BUGL_GAUSSIAN_POINT_3D_INSTANTIATE
#define BUGL_GAUSSIAN_POINT_3D_INSTANTIATE(T) \
template class bugl_gaussian_point_3d<T >

#endif // bugl_gaussian_point_3d_txx_
