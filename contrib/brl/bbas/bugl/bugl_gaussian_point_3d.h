#ifndef GAUSSIAN_POINT_3D_H_
#define GAUSSIAN_POINT_3D_H_
//:
// \file
// \author Kongbin Kang
// \brief A class for 3d point with Gaussian distribution
//
// \verbatim
//  Modifications
//   10 Sept 2004 Peter Vanroose - pass arguments as "const&" instead of as "&"
// \endverbatim

#include "bugl_uncertainty_point_3d.h"
#include <vnl/vnl_matrix_fixed.h>

template<class T>
class bugl_gaussian_point_3d : public bugl_uncertainty_point_3d<T>
{
  vnl_matrix_fixed<T, 3, 3> sigma_;
  vnl_matrix_fixed<T, 3, 3> sigma_inv_;
  T det_;
 public:
  bugl_gaussian_point_3d() : bugl_uncertainty_point_3d<T>() {}
  bugl_gaussian_point_3d(vgl_point_3d<T> const& p, vnl_matrix_fixed<T, 3, 3> const& s)
    : bugl_uncertainty_point_3d<T>(p) { set_covariant_matrix(s); }
  bugl_gaussian_point_3d(T x, T y, T z, vnl_matrix_fixed<T, 3, 3> const& s)
    : bugl_uncertainty_point_3d<T>(x, y, z) { set_covariant_matrix(s); }
  ~bugl_gaussian_point_3d() {}

  vnl_matrix_fixed<T, 3, 3> get_covariant_matrix() const { return sigma_; }
  void set_covariant_matrix(vnl_matrix_fixed<T, 3, 3> const& sigma);

  //: probability density at point p
  T prob_at(vgl_point_3d<T> const& p) const;
};

#define BUGL_GAUSSIAN_POINT_3D_INSTANTIATE(T) extern "please include bugl/gaussian_point_3d_.txx first"
#endif
