// This is vpgl/algo/vpgl_bundle_adjust_lsqr.h
#ifndef vpgl_bundle_adjust_lsqr_h_
#define vpgl_bundle_adjust_lsqr_h_
//:
// \file
// \brief Bundle adjustment sparse least squares base class
// \author Matt Leotta
// \date April 18, 2005
//
// \verbatim
//  Modifications
//   Mar 23, 2010  MJL - Split off base class and moved to its own file
// \endverbatim


#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_sparse_lst_sqr_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: base class bundle adjustment sparse least squares function
class vpgl_bundle_adjust_lsqr : public vnl_sparse_lst_sqr_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because they require finite points
  //       to measure projection error
  vpgl_bundle_adjust_lsqr(unsigned int num_params_per_a,
                          unsigned int num_params_per_b,
                          unsigned int num_params_c,
                          std::vector<vgl_point_2d<double> >  image_points,
                          const std::vector<std::vector<bool> >& mask);

  //: Constructor
  //  Each image point is assigned an inverse covariance (error projector) matrix
  // \note image points are not homogeneous because they require finite points
  //       to measure projection error
  vpgl_bundle_adjust_lsqr(unsigned int num_params_per_a,
                          unsigned int num_params_per_b,
                          unsigned int num_params_c,
                          const std::vector<vgl_point_2d<double> >& image_points,
                          const std::vector<vnl_matrix<double> >& inv_covars,
                          const std::vector<std::vector<bool> >& mask);

  // Destructor
  ~vpgl_bundle_adjust_lsqr() override = default;

  //: Compute all the reprojection errors
  //  Given the parameter vectors a, b, and c, compute the vector of residuals e.
  //  e has been sized appropriately before the call.
  //  The parameters in for each camera are in a.
  //  The 3D point parameters are in b.
  //  Global parameters are in c.
  void f(vnl_vector<double> const& a,
                 vnl_vector<double> const& b,
                 vnl_vector<double> const& c,
                 vnl_vector<double>& e) override;

  //: Compute the residuals from the ith component of a, the jth component of b, and all of c.
  //  This function is not normally used because f() has a
  //  self-contained efficient implementation.
  //  It is used for finite-differencing if the gradient is marked as unavailable
  void fij(int i, int j,
                   vnl_vector<double> const& ai,
                   vnl_vector<double> const& bj,
                   vnl_vector<double> const& c,
                   vnl_vector<double>& fij) override;

  //: Compute the sparse Jacobian in block form.
  void jac_blocks(vnl_vector<double> const& a,
                          vnl_vector<double> const& b,
                          vnl_vector<double> const& c,
                          std::vector<vnl_matrix<double> >& A,
                          std::vector<vnl_matrix<double> >& B,
                          std::vector<vnl_matrix<double> >& C) override;


  /* parent class has different signatures for these functions */
  using vnl_sparse_lst_sqr_function::jac_Aij;
  using vnl_sparse_lst_sqr_function::jac_Bij;
  using vnl_sparse_lst_sqr_function::jac_Cij;

  //: compute the Jacobian Aij
  virtual void jac_Aij(unsigned int i,
                       unsigned int j,
                       vnl_double_3x4 const& Pi,
                       vnl_vector<double> const& ai,
                       vnl_vector<double> const& bj,
                       vnl_vector<double> const& c,
                       vnl_matrix<double>& Aij) = 0;

  //: compute the Jacobian Bij
  virtual void jac_Bij(unsigned int i,
                       unsigned int j,
                       vnl_double_3x4 const& Pi,
                       vnl_vector<double> const& ai,
                       vnl_vector<double> const& bj,
                       vnl_vector<double> const& c,
                       vnl_matrix<double>& Bij) = 0;

  //: compute the Jacobian Cij
  virtual void jac_Cij(unsigned int i,
                       unsigned int j,
                       vnl_double_3x4 const& Pi,
                       vnl_vector<double> const& ai,
                       vnl_vector<double> const& bj,
                       vnl_vector<double> const& c,
                       vnl_matrix<double>& Cij) = 0;

  //: Use an M-estimator to compute weights
  void compute_weight_ij(int i, int j,
                         vnl_vector<double> const& ai,
                         vnl_vector<double> const& bj,
                         vnl_vector<double> const& c,
                         vnl_vector<double> const& fij,
                         double& weight) override;

  //: set the residual scale for the robust estimation
  void set_residual_scale(double scale) { scale2_ = scale*scale; }


  //: construct the \param j-th 3D point from parameter vector \param b and \param c
  vgl_homg_point_3d<double>
  param_to_point(int j,
                 const vnl_vector<double>& b,
                 const vnl_vector<double>& c) const
  {
    return param_to_point(j, b.data_block() + index_b(j), c);
  }

  //: construct the \param j-th perspective camera from a pointer to the j-th parameter of \param bj and parameters \param c
  virtual vgl_homg_point_3d<double>
  param_to_point(int j,
                 const double* bj,
                 const vnl_vector<double>& c) const = 0;

  //: construct the \param j-th 3D point from parameter vector \param b and \param c
  vnl_vector_fixed<double,4>
  param_to_pt_vector(int j,
                     const vnl_vector<double>& b,
                     const vnl_vector<double>& c) const
  {
    return param_to_pt_vector(j, b.data_block() + index_b(j), c);
  }

  //: construct the \param j-th perspective camera from a pointer to the j-th parameter of \param bj and parameters \param c
  virtual vnl_vector_fixed<double,4>
  param_to_pt_vector(int j,
                     const double* bj,
                     const vnl_vector<double>& c) const = 0;

  //: construct the \param i-th perspective camera from parameter vector \param a
  vpgl_perspective_camera<double>
  param_to_cam(int i,
               const vnl_vector<double>& a,
               const vnl_vector<double>& c) const
  {
    return param_to_cam(i, a.data_block()+index_a(i), c);
  }

  //: construct the \param i-th perspective camera from a pointer to the i-th parameter of \param ai and parameters \param c
  virtual vpgl_perspective_camera<double>
  param_to_cam(int i,
               const double* ai,
               const vnl_vector<double>& c) const = 0;

  //: construct the \param i-th perspective camera matrix from parameter vectors \param a and \param c
  vnl_double_3x4 param_to_cam_matrix(int i,
                                     const vnl_vector<double>& a,
                                     const vnl_vector<double>& c) const
  {
    return param_to_cam_matrix(i, a.data_block()+index_a(i), c);
  }

  //: compute the 3x4 matrix of camera \param i from a pointer to the i-th parameter of \param ai and parameters \param c
  virtual vnl_double_3x4 param_to_cam_matrix(int i,
                                             const double* ai,
                                             const vnl_vector<double>& c) const = 0;

  //: reset the weights
  void reset()
  {
    iteration_count_ = 0;
  }


  //---------------------------------------------------------------------------
  // Static helper functions

  //: Fast conversion of rotation from Rodrigues vector to matrix
  static vnl_double_3x3 rod_to_matrix(vnl_vector<double> const& r);

  //: compute the 2x3 Jacobian of camera projection with respect to point location df/dpt where $f(pt) = P*pt$
  static void jac_inhomg_3d_point(vnl_double_3x4 const& P,
                                  vnl_vector<double> const& pt,
                                  vnl_matrix<double>& J);

  //: compute the 2x3 Jacobian of camera projection with respect to camera center df/dC where $f(C) = [M | -M*C]*pt$
  static void jac_camera_center(vnl_double_3x3 const& M,
                                vnl_vector<double> const& C,
                                vnl_vector<double> const& pt,
                                vnl_matrix<double>& J);

  //: compute the 2x3 Jacobian of camera projection with respect to camera rotation df/dr where $f(r) = K*rod_to_matrix(r)*[I | -C]*pt$
  //  Here r is a Rodrigues vector, K is an upper triangular calibration matrix
  static void jac_camera_rotation(vnl_double_3x3 const& K,
                                  vnl_vector<double> const& C,
                                  vnl_vector<double> const& r,
                                  vnl_vector<double> const& pt,
                                  vnl_matrix<double>& J);


 protected:
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
  //: The Cholesky factored inverse covariances for each image point
  std::vector<vnl_matrix<double> > factored_inv_covars_;
  //: Flag to enable covariance weighted errors
  bool use_covars_;
  //: The square of the scale of the robust estimator
  double scale2_;

  int iteration_count_;
};


#endif // vpgl_bundle_adjust_lsqr_h_
