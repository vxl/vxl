// This is core/vgl/algo/vgl_fit_sphere_3d.h
#ifndef vgl_fit_sphere_3d_h_
#define vgl_fit_sphere_3d_h_
//:
// \file
// \brief Fits a sphere to a set of 3D points
// \author Joseph L. Mundy
// \date June 8, 2015
//
//  Solve linear system as an initial condition for non-linear Levenberg Marquadt
//  Parameters are radius and center
//  Initial experiments (see test case) indicate that there isn't much
//  difference between the linear algorithm and the non-linear Levenberg Marquardt
//  algorithm. Both options are available to suit specific applications. The
//  non-linear algorithm first does a linear fit to obtain and inital guess.
//
// \verbatim
//  Modifications
//   none
// \endverbatim

// The linear algorithm seeks to minimize the error e = Sum|ri^2 - r^2|
// expanding (xi-x0)^2 + (yi-y0)^2 + (zi-z0)^2 - r^2
// = rho - 2(xix0 + yiy0 + ziz0) + (xi^2 + yi^2 + zi^2)
// where rho = (x0^2 + y0^2 + z0^2) - r^2
//  form three matrices
//         [        ...            ]            [        ...          ]         [x0 ]
//  Anx4 = [ -2xix0 -2yiy0 -2ziz0 1],   Bnx1 =  [ -xi^2  -yi^2  -zi^2 ], P4x1 = [y0 ]
//         [        ...            ]            [        ...          ]         [z0 ]
//                                                                              [rho]
//  Then solve the linear system,  AP - B = 0, using SVD.
//
// For the non-linear algorithm, the residuals are ei = ri - r;
// The Jacobian matrix is given by,
//
//             [                 ...                 ]
//     Jnx4 = -[(xi-x0)/ri  (yi-y0)/ri  (zi-z0)/ri  1]
//             [                 ...                 ]
//
#include <vector>
#include <iosfwd>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_sphere_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class vgl_fit_sphere_3d
{
  // Data Members--------------------------------------------------------------
 protected:
  std::vector<vgl_homg_point_3d<T> > points_;
  vgl_sphere_3d<T> sphere_lin_;
  vgl_sphere_3d<T> sphere_non_lin_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

   vgl_fit_sphere_3d() = default;

   vgl_fit_sphere_3d(std::vector<vgl_point_3d<T> > points);

  ~vgl_fit_sphere_3d() = default;

  // Operations---------------------------------------------------------------

  //: add a point to point set
  void add_point(vgl_point_3d<T> const &p);
  void add_point(const T x, const T y, const T z);

  //: clear internal data
  void clear();

  //: fit a sphere to the stored points using a linear method
  // returns the average distance from the points to the sphere
  // used as an initial condition for Levenberg Marquardt
  // error conditions are reported on outstream
  T fit_linear(std::ostream* outstream=nullptr);

  //:fits a sphere to the stored points using a linear method
  bool fit_linear(const T error_marg, std::ostream* outstream=nullptr);

  //:fits a sphere nonlinearly to the stored points using Levenberg Marquardt
  // returns the average distance from the points to the sphere
  T fit(std::ostream* outstream=nullptr, bool verbose = false);

  //:fits a sphere nonlinearly to the stored points using Levenberg Marquardt
  bool fit(const T error_marg, std::ostream* outstream=nullptr, bool verbose = false);

// Data Access---------------------------------------------------------------

  std::vector<vgl_point_3d<T> > get_points() const;

  //: appropriate fit function should be called first to get the sphere corresponding to the points
  vgl_sphere_3d<T>& get_sphere_linear_fit() {return sphere_lin_;}
  vgl_sphere_3d<T>& get_sphere_nonlinear_fit() {return sphere_non_lin_;}
};

#define VGL_FIT_SPHERE_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_fit_sphere_3d.hxx first"

#endif // vgl_fit_sphere_3d_h_
