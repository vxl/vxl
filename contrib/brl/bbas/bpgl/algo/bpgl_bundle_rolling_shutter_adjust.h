// This is bbas/bpgl/algo/bpgl_bundle_rolling_shutter_adjust.h
#ifndef bpgl_bundle_rolling_shutter_adjust_h_
#define bpgl_bundle_rolling_shutter_adjust_h_
//:
// \file
// \brief Bundle adjustment sparse least squares functions
// \author Matt Leotta
// \date April 18, 2005

#include <iostream>
#include <vector>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_lst_sqr_function.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: Computes the residuals for bundle adjustment given that the cameras share a fixed internal calibration, shared rolling rate which needs to be estimated
class bpgl_bundle_rolling_shutter_adj_lsqr : public vnl_sparse_lst_sqr_function
{
 public:
  //: Constructor
  // \note image points are not homogeneous because they require finite points to measure projection error
  bpgl_bundle_rolling_shutter_adj_lsqr(const std::vector<vpgl_calibration_matrix<double> >& K,
                                       const std::vector<vgl_point_2d<double> >& image_points,
                                       const std::vector<std::vector<bool> >& mask,
                                       double rolling_rate,
                                       bool use_confidence_weights = true);

  //: Constructor
  //  Each image point is assigned an inverse covariance (error projector) matrix
  // \note image points are not homogeneous because they require finite points to measure projection error
  bpgl_bundle_rolling_shutter_adj_lsqr(const std::vector<vpgl_calibration_matrix<double> >& K,
                                       const std::vector<vgl_point_2d<double> >& image_points,
                                       const std::vector<vnl_matrix<double> >& inv_covars,
                                       const std::vector<std::vector<bool> >& mask,
                                       bool use_confidence_weights = true);

  // Destructor
  ~bpgl_bundle_rolling_shutter_adj_lsqr() override = default;

  //: Compute all the reprojection errors
  //  Given the parameter vectors a and b, compute the vector of residuals e.
  //  e has been sized appropriately before the call.
  //  The parameters in a for each camera are {wx, wy, wz, tx, ty, tz}
  //  where w is the Rodrigues vector of the rotation and t is the translation.
  //  The parameters in b for each 3D point are {px, py, pz}
  //  the non-homogeneous position.
  virtual void f(vnl_vector<double> const& a, vnl_vector<double> const& b,
                 vnl_vector<double>& e);

  //: Compute the residuals from the ith component of a and the jth component of b.
  //  This is not normally used because f() has a self-contained efficient implementation
  //  It is used for finite-differencing if the gradient is marked as unavailable
  virtual void fij(int i, int j, vnl_vector<double> const& ai,
                   vnl_vector<double> const& bj, vnl_vector<double>& fij);

  //: Compute the sparse Jacobian in block form.
  virtual void jac_blocks(vnl_vector<double> const& a, vnl_vector<double> const& b,
                          std::vector<vnl_matrix<double> >& A,
                          std::vector<vnl_matrix<double> >& B);

  //: compute the Jacobian Aij
  void jac_Aij(vnl_double_3x4 const& Pi, vnl_double_3x3 const& K,
               vnl_vector<double> const& ai, vnl_vector<double> const& bj,
               double v_k,
               vnl_matrix<double>& Aij);

  //: compute the Jacobian Bij
  void jac_Bij(vnl_double_3x4  Pi,
               vnl_vector<double> const& ai,
               vnl_vector<double> const& bj,
               double v_k,
               vnl_matrix<double>& Bij);

  //: construct the jth 3D point from parameter vector b
  vgl_homg_point_3d<double> param_to_point(int j, const vnl_vector<double>& b) const
  {
    const double* d = b.data_block() + index_b(j);
    return {d[0], d[1], d[2]};
  }

  //: construct the ith perspective camera from parameter vector a
  vpgl_perspective_camera<double> param_to_cam(int i, const vnl_vector<double>& a) const
  {
    return param_to_cam(i, a.data_block()+index_a(i));
  }

  vpgl_perspective_camera<double> param_to_cam(int i, const double* d) const
  {
    vnl_vector<double> w(d,3);
    vgl_homg_point_3d<double> t(d[3], d[4], d[5]);
    return vpgl_perspective_camera<double>(K_[i],t,vgl_rotation_3d<double>(w));
  }

  //: construct the ith perspective camera matrix from parameter vector \param a
  //  Bypass vpgl for efficiency
  vnl_double_3x4 param_to_cam_matrix(int i, const vnl_vector<double>& a) const
  {
    return param_to_cam_matrix(i, a.data_block()+index_a(i));
  }
  vnl_double_4x4 param_to_motion_matrix(int i, const vnl_vector<double>& a, double r, double v) const
  {
    return param_to_motion_matrix(i, a.data_block()+index_a(i),r,v);
  }
  vnl_vector<double> param_to_motion(int i, const vnl_vector<double>& a) const
  {
    return param_to_motion(i, a.data_block()+index_a(i));
  }

  vnl_vector<double> param_to_motion(int /*i*/, const double* d) const
  {
    vnl_vector<double> w(6);
    w[0]=d[6];w[1]=d[7];w[2]=d[8];
    w[3]=d[9];w[4]=d[10];w[5]=d[11];
    return w;
  }

  vnl_double_3x4 param_to_cam_matrix(int i, const double* d) const
  {
    vnl_double_3x3 M = Km_[i]*rod_to_matrix(d);
    vnl_double_3x4 P;
    P.set_columns(0, M.as_ref());
    const vnl_vector_ref<double> c(3,const_cast<double*>(d+3));
    P.set_column(3,-(M*c));
    return P;
  }

  //: convert params into first order motion
  vnl_double_4x4 param_to_motion_matrix(int i, const double* data, double r, double v) const;

  //: Fast conversion of rotation from Rodrigues vector to matrix
  vnl_matrix_fixed<double,3,3> rod_to_matrix(const double* r) const;

  //: conversion of vector into skew symmetric vector
  vnl_matrix_fixed<double,3,3> vector_to_skewmatrix(const double* r) const;

  //: Create the parameter vector \p a from a vector of cameras
  static vnl_vector<double>
    create_param_vector(const std::vector<vpgl_perspective_camera<double> >& cameras,
                        const std::vector<vnl_vector<double> > & motion);

  //: Create the parameter vector \p b from a vector of 3D points
  static vnl_vector<double>
    create_param_vector(const std::vector<vgl_point_3d<double> >& world_points);


  void reset() { iteration_count_ = 0; for (double & weight : weights_) weight = 1.0; }

  //: return the current weights
  std::vector<double> weights() const { return weights_; }

 protected:
  //: The fixed internal camera calibration
  std::vector<vpgl_calibration_matrix<double> > K_;
  //: The fixed internal camera calibration in matrix form
  std::vector<vnl_double_3x3> Km_;
  //: The corresponding points in the image
  std::vector<vgl_point_2d<double> > image_points_;
  //: The Cholesky factored inverse covariances for each image point
  std::vector<vnl_matrix<double> > factored_inv_covars_;
  //: Flag to enable covariance weighted errors
  bool use_covars_;
  //: Flag to enable confidence weighting
  bool use_weights_;
  //: The corresponding points in the image
  std::vector<double> weights_;
  int iteration_count_;
  double rolling_rate_;
};


//: Static functions for bundle adjustment
class bpgl_bundle_rolling_shutter_adjust
{
 public:
  //: Constructor
  bpgl_bundle_rolling_shutter_adjust();
  //: Destructor
  ~bpgl_bundle_rolling_shutter_adjust();

  void set_use_weights(bool use_weights) { use_weights_ = use_weights; }
  void set_use_gradient(bool use_gradient) { use_gradient_ = use_gradient; }

  //: Return the ending error
  double end_error() const { return end_error_; }
  //: Return the starting error
  double start_error() const { return start_error_; }
  //: Return the number of iterations
  int num_iterations() const { return num_iterations_; }

  //: Return the raw camera parameters
  const vnl_vector<double>& cam_params() const { return a_; }
  //: Return the raw world point parameters
  const vnl_vector<double>& point_params() const { return b_; }

  //: Bundle Adjust
  bool optimize(std::vector<vpgl_perspective_camera<double> >& cameras,
                std::vector<vnl_vector<double> > & motion,
                double & r,
                std::vector<vgl_point_3d<double> >& world_points,
                const std::vector<vgl_point_2d<double> >& image_points,
                const std::vector<std::vector<bool> >& mask);

  //: Write cameras and points to a file in VRML 2.0 for debugging
  static void write_vrml(const std::string& filename,
                         std::vector<vpgl_perspective_camera<double> >& cameras,
                         std::vector<vgl_point_3d<double> >& world_points);

 private:
  //: The bundle adjustment error function
  bpgl_bundle_rolling_shutter_adj_lsqr* ba_func_;
  //: The last camera parameters
  vnl_vector<double> a_;
  //: The last point parameters
  vnl_vector<double> b_;

  bool use_weights_;
  bool use_gradient_;

  double start_error_;
  double end_error_;
  int num_iterations_;
};

#endif // bpgl_bundle_rolling_shutter_adjust_h_
