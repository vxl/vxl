#ifndef GAUSSIAN_POINT_3D_H_
#define GAUSSIAN_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// A class for 3d point with Gaussian distribution
// 
// \verbatim
// \endverbatim

#include "bugl_uncertainty_point_3d.h"
#include <vnl/vnl_matrix_fixed.h>
template<class T>
class bugl_gaussian_point_3d:public bugl_uncertainty_point_3d<T>
{
private:
  vnl_matrix_fixed<T, 3, 3> sigma_;
  vnl_matrix_fixed<T, 3, 3> sigma_inv_;
  
public:
  bugl_gaussian_point_3d();
  bugl_gaussian_point_3d(vgl_point_3d<T> &p, vnl_matrix_fixed<T, 3, 3> &s);
  bugl_gaussian_point_3d(T x, T y, T z, vnl_matrix_fixed<T, 3, 3> & s);
  ~bugl_gaussian_point_3d() {}

  vnl_matrix_fixed<T, 3, 3> get_covariant_matrix() { return sigma_;}
  void set_point(vgl_point_3d<T> &p) { this->set(p.x(), p.y(), p.z());}
  void set_covariant_matrix(vnl_matrix_fixed<T, 3, 3> & sigma);
};

#define BUGL_GAUSSIAN_POINT_3D_INSTANTIATE(T) extern "please include bugl/gaussian_point_3d_.txx first"
#endif
