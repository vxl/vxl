#ifndef bugl_gaussian_point_2d_txx_
#define bugl_gaussian_point_2d_txx_

#include "bugl_gaussian_point_2d.h"
#include <vnl/vnl_inverse.h>

template<class T>
bugl_gaussian_point_2d<T>::bugl_gaussian_point_2d(T x, T y, vnl_matrix_fixed<T, 2, 2> & s)
: bugl_uncertainty_point_2d<T>(x, y)
{
  set_covariant_matrix(s);
}

template<class T>
bugl_gaussian_point_2d<T>::bugl_gaussian_point_2d(\
  vgl_point_2d<T> &p, vnl_matrix_fixed<T, 2, 2> &s)\
: bugl_uncertainty_point_2d<T>(p)
{
  set_covariant_matrix(s);
}

template<class T>
bugl_gaussian_point_2d<T>::bugl_gaussian_point_2d()
: bugl_uncertainty_point_2d<T>()
{
}

template<class T>
void bugl_gaussian_point_2d<T>::set_point(vgl_point_2d<T> &p)
{ 
    this->set(p.x(), p.y());
}

template<class T>
void bugl_gaussian_point_2d<T>::set_covariant_matrix(\
  vnl_matrix_fixed<T, 2, 2> &s)
{
  sigma_ = s;
  sigma_inv_ = vnl_inverse(s);
}
//----------------------------------------------------------------------------
#undef BUGL_GAUSSIAN_POINT_2D_INSTANTIATE
#define BUGL_GAUSSIAN_POINT_2D_INSTANTIATE(T) \
template class bugl_gaussian_point_2d<T >

#endif // bugl_gaussian_point_2d_txx_
