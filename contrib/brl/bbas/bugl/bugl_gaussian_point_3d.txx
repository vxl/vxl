#ifndef bugl_gaussian_point_3d_txx_
#define bugl_gaussian_point_3d_txx_

#include "bugl_gaussian_point_3d.h"
#include <vnl/vnl_inverse.h>

template<class T>
bugl_gaussian_point_3d<T>::bugl_gaussian_point_3d(T x, T y, T z, vnl_matrix_fixed<T, 3, 3> & s) :
bugl_uncertainty_point_3d<T>(x, y, z)
{
  sigma_ = s;
  sigma_inv_ = vnl_inverse(s);
}

//----------------------------------------------------------------------------
#undef BUGL_GAUSSIAN_POINT_3D_INSTANTIATE
#define BUGL_GAUSSIAN_POINT_3D_INSTANTIATE(T) \
template class bugl_gaussian_point_3d<T >

#endif // bugl_gaussian_point_3d_txx_
