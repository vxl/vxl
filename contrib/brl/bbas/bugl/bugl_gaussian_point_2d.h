#ifndef gaussian_point_2d_h_
#define gaussian_point_2d_h_
//:
// \file
// \author Kongbin Kang
// \brief A class for 2d point with Gaussian distribution
//
// \verbatim
//  Modifications
//   10 Sept 2004 Peter Vanroose - pass arguments as "const&" instead of as "&"
// \endverbatim

#include "bugl_uncertainty_point_2d.h"
#include <vnl/vnl_matrix_fixed.h>

template<class T>
class bugl_gaussian_point_2d : public bugl_uncertainty_point_2d<T>
{
  vnl_matrix_fixed<T, 2, 2> sigma_;
  vnl_matrix_fixed<T, 2, 2> sigma_inv_;
  T det_;
 public:
  bugl_gaussian_point_2d() : bugl_uncertainty_point_2d<T>() {}
  bugl_gaussian_point_2d(vgl_point_2d<T> const& p, vnl_matrix_fixed<T, 2, 2> const& s)
    : bugl_uncertainty_point_2d<T>(p) { set_covariant_matrix(s); }
  bugl_gaussian_point_2d(T x, T y, vnl_matrix_fixed<T, 2, 2> const& s)
    : bugl_uncertainty_point_2d<T>(x, y) { set_covariant_matrix(s); }
  ~bugl_gaussian_point_2d() {}

  vnl_matrix_fixed<T, 2, 2> get_covariant_matrix() const { return sigma_; }
  void set_covariant_matrix(vnl_matrix_fixed<T, 2, 2> const& s);

  //: probability density at point p
  T prob_at(vgl_point_2d<T> const& p) const;
};

#define BUGL_GAUSSIAN_POINT_2D_INSTANTIATE(T) extern "please include bugl/gaussian_point_2d.txx first"

#endif
