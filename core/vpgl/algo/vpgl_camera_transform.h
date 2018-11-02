// This is core/vpgl/algo/vpgl_camera_transform.h
#ifndef vpgl_camera_transform_h_
#define vpgl_camera_transform_h_
//:
// \file
// \brief Methods for computing a fixed transformation that moves and orients a camera so that given 3-d points project to the given 2-d image positions
// \author Ozge C. Ozcanli
// \date Jan 13, 2014

#include <utility>
#include <vpgl/vpgl_proj_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_rotation_3d.h>
#include <vnl/vnl_least_squares_function.h>
#include <vpgl/vpgl_calibration_matrix.h>

//: The cost function to be used by a Lev-Marq minimization routine, compute the residuals for each correspondence
//  6 dimensional param vector, x[0], x[1], x[2] constitute Rodriguez vector of rotation, x[3], x[4], x[5] constitute the translation of the camera
/// the fixed and the same rotation and translation are applied to all the cameras
class vpgl_camera_transform_f : public vnl_least_squares_function
{
 public:
  //: Constructor. The source image is mapped to the destination frame by dt. nbins is the number of histogram bins used to compute entropies.
  vpgl_camera_transform_f(unsigned cnt_residuals, unsigned n_unknowns,
                          const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                          std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >  cam_ids_img_pts,
                          std::vector<vnl_vector_fixed<double, 4> >  pts_3d, bool minimize_R = true);

  //: The main function.
  //  Given the parameter vector x, compute the vector of residuals fx.
  //  Fx has been sized appropriately before the call.
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx) override;

  //: Calculate the Jacobian, given the parameter vector x using forward differencing
  //virtual void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

  //: conversion of rodriguez params to R
  virtual vnl_matrix_fixed<double,3,3> rod_to_matrix(double r0, double r1, double r2);
  virtual void compute_cams(vnl_vector<double> const& x, std::vector<vpgl_perspective_camera<double> >& output_cams);
  virtual void compute_cams_selective(vnl_vector<double> const& x, std::vector<vpgl_perspective_camera<double> >& output_cams);

  // for each 3d point j, there are n possible 2d image projections
  std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > > cam_ids_img_pts_;
  std::vector<vnl_vector_fixed<double, 4> > pts_3d_;

  std::vector<vpgl_calibration_matrix<double> > Ks_;
  std::vector<vnl_matrix_fixed<double, 3, 3> > Rs_;
  std::vector<vgl_point_3d<double> > Cs_;
  std::vector<vpgl_perspective_camera<double>  > input_cams_;

  bool minimize_R_;

 protected:

};

class vpgl_camera_transform
{
 public:
  ~vpgl_camera_transform();

  //: apply fixeld transformation -- R_new = R*R_old
  static void apply_fixed_transformation(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                         vnl_matrix_fixed<double,3,3>& R, vgl_point_3d<double>& t,
                                         std::vector<vpgl_perspective_camera<double>  >& output_cams);

  //: compute the fixed transformation as R and camera center
  static bool compute_fixed_transformation(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                           const std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                           const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                           std::vector<vpgl_perspective_camera<double>  >& output_cams);

  //: sample offsets for camera centers in a box with the given dimensions (e.g. plus/minus dim_x) in meters
  static std::vector<vnl_vector_fixed<double, 3> > sample_centers(double dim_x, double dim_y, double dim_z, double step);

  //: compute the fixed transformation by sampling centers in a given box and then optimizing for rotation
  static bool compute_fixed_transformation_sample(const std::vector<vpgl_perspective_camera<double>  >& input_cams,
                                                  const std::vector< std::vector< std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                                  const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                                  std::vector<vpgl_perspective_camera<double>  >& output_cams);

  static void normalize_img_pts(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                std::vector<vpgl_perspective_camera<double> >& input_cams_norm,
                                std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts_norm);

  //: normalize the points using the inverse of the K matrix
  static void K_normalize_img_pts(const std::vector<vpgl_perspective_camera<double> >& input_cams, vnl_matrix_fixed<double, 3, 3> const& input_correspondence_covariance,
                                const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                std::vector< std::vector < std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > >& cam_ids_img_pts_norm);

  // compute both R and t -- // TODO: the formulation for the initial transformation need to be updated to multiple R from left (not right as in the current version)
  static bool compute_initial_transformation(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                             const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                             const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                             std::vector<vpgl_perspective_camera<double> >& output_cams);

  // compute only t
  static bool compute_initial_transformation_t(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                               const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                               const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                               std::vector<vpgl_perspective_camera<double> >& output_cams);

  // compute only R -- // TODO: the formulation for the initial transformation need to be updated to multiple R from left (not right as in the current version)
  static bool compute_initial_transformation_R(const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                               const std::vector< std::vector < std::pair<vnl_vector_fixed<double, 2>, unsigned> > >& cam_ids_img_pts,
                                               const std::vector<vnl_vector_fixed<double, 4> >& pts_3d,
                                               std::vector<vpgl_perspective_camera<double> >& output_cams);

  static bool normalize_to_rotation_matrix(const vnl_matrix_fixed<double, 3, 3>& R, vnl_matrix_fixed<double, 3, 3>& R_norm);
  // use quaternions
  static bool normalize_to_rotation_matrix_q(const vnl_matrix_fixed<double, 3, 3>& R, vnl_matrix_fixed<double, 3, 3>& R_norm);

  //: pass the ids of cams in the input_cams vector, this method computes the variance between these two using their image correspondences (K normalized image points)
  //  correspondence_covariance is the 3x3 covariance matrix of the points in the K normalized image space
  static bool compute_covariance(unsigned i, unsigned j, const std::vector<vpgl_perspective_camera<double> >& input_cams,
                                 const std::vector< std::vector < std::pair< std::pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > >& cam_ids_img_pts,
                                 vnl_matrix_fixed<double, 3, 3>& rot_variance);

 private:
  //: constructor private - static methods only
  vpgl_camera_transform();
};

#endif // vpgl_camera_transform_h_
