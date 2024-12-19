// This is core/vgl/algo/vgl_fit_cylinder_3d.h
#ifndef vgl_fit_cylinder_3d_h_
#define vgl_fit_cylinder_3d_h_
//:
// \file
// \brief Fits a cylinder to a set of 3D points,{ Xi }
// \author Joseph L. Mundy
// \date June 8, 2015
//
// The linear algorithm seeks to minimize the error e = Sum(ri^2 - r^2)^2
// where, r is the cylinder radius and ri^2 = (C - Xi)^T P(C - Xi)
// P is the projection operator, P = (I - W W^T) and W is the cylinder axis unit vector
// de/dr = Sum(ri^2 - r^2)= 0. It follows that r^2 = 1/n Sum(ri^2).
// Thus  r = Sqrt(1/n Sum(ri^2))
//
// The previous solution assumes that W is known, which is often the case (e.g.,along z)
// The center is found by projecting the points onto a plane perpendicular to W
// Chose two orthogonal vectors in the perpendicular plane, U, V, such that U x V = W
// Let P(Xi) = mu_i U + nu_i V, where mu_i = U.Xi and nu_i = v.Xi Let ku = C.U and kv = C.V.
// Define the covariance matrix of the 2-d projections of Xi as,
//                         _                          _
//                        |  <mu_i^2>     <mu_i nu_i>  |  where < qi > = 1/n Sum(qi)
//              [covar] = |                            |
//                        |  <mu_i nu_i>    <nu_i>     |
//                         -                          -
//                 _   _        _                      _
//                | ku  |    1 | <mu_i(mu_i^2+ nu_i^2)> |
//  Then,  [covar]|     | =  - |                        |
//                | kv  |    2 | <nu_i(mu_i^2+ nu_i^2)> |
//                 -   -        -                      -
//  has a unique linear solution unless [covar] is singular
//
//  Determining W is not possible in a closed form solution and involves root finding leading to a
//  brittle algorithm. A better approach is to quantize the W orientations
//    (e.g. 5 degree intervals on the hemisphere)
//  and then find the W that minimizes e = Sum(ri^2 - r^2)^2. Can be costly with a large number of samples.
//  If the cylinder has good uniform point coverage then W can be estimated from the points by finding the
//  principal eigenvector of covar(X) = <Xi Xi^T>
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vector>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_cylinder_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_fit_cylinder_3d
{
  // Data Members--------------------------------------------------------------
protected:
  std::vector<vgl_homg_point_3d<T>> points_;
  vgl_cylinder_3d<T> cylinder_;

public:
  // Constructors/Initializers/Destructors-------------------------------------

  vgl_fit_cylinder_3d() = default;

  vgl_fit_cylinder_3d(std::vector<vgl_point_3d<T>> points);

  ~vgl_fit_cylinder_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void
  add_point(const vgl_point_3d<T> & p);
  void
  add_point(const T x, const T y, const T z);

  //: clear internal data
  void
  clear();

  //: fit a cylinder to the stored points using a linear method, where W is known
  // returns the average distance from the points to the cylinder
  // error conditions are reported on outstream
  T
  fit(const vgl_vector_3d<T> & W, std::ostream * outstream = nullptr, bool verbose = false);

  //: fit a cylinder to the stored points using a linear method, where W is known
  bool
  fit(const vgl_vector_3d<T> & W, const T error_marg, std::ostream * outstream = nullptr, bool verbose = false);

  //: fit a cylinder to the stored points using a linear method, where W
  // is estimated as the eigenvector of the largest eigenvalue of the input pointset
  // returns the average distance from the points to the cylinder
  // error conditions are reported on outstream
  T
  fit(std::ostream * outstream = nullptr, bool verbose = false);

  //: fit a cylinder to the stored points using a linear method, where W is computed from the points
  bool
  fit(const T error_marg, std::ostream * outstream = nullptr, bool verbose = false);

  // Data Access---------------------------------------------------------------

  std::vector<vgl_point_3d<T>>
  get_points() const;
  vgl_cylinder_3d<T> &
  get_cylinder()
  {
    return cylinder_;
  }
};

#define VGL_FIT_CYLINDER_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_cylinder_3d.hxx first"

#endif // vgl_fit_cylinder_3d_h_
