#ifndef breg3d_ekf_camera_optimizer_h_
#define breg3d_ekf_camera_optimizer_h_
//:
// \file
// \brief Camera optimizer using Extended Kalman Filter
//
// \author Daniel Crispell
// \date Mar 01, 2008
// \verbatim
//  Modifications
//   Mar 25 2008 dec  moved to contrib/dec/breg3d
//   Aug 09 2010 jlm  moved to brl/bseg/bvxm/breg3d
// \endverbatim


#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <vil/vil_image_view_base.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

#include "breg3d_ekf_camera_optimizer_state.h"
#include "breg3d_homography_generator.h"

//: Container for algorithms related to optimizing a perspective camera's pose based a voxel grid
class breg3d_ekf_camera_optimizer
{
 public:
  breg3d_ekf_camera_optimizer(double pos_var_predict, double rot_var_predict,
                            double pos_var_measure, double rot_var_measure,
                            double homography_var, double homography_var_t,
                            bool use_gps = true, bool use_expected = true, bool use_proj_homography = false);

  ~breg3d_ekf_camera_optimizer();

  breg3d_ekf_camera_optimizer_state optimize(bvxm_voxel_world_sptr const& vox_world,
                                           vil_image_view_base_sptr &prev_img, bvxm_image_metadata &curr_img,
                                           breg3d_ekf_camera_optimizer_state &prev_state,
                                           const std::string& apm_type = "apm_mog_grey", unsigned bin_idx = 0);

 protected:
  //breg3d_ekf_camera_optimizer() : homography_gen_(0) {}

    breg3d_ekf_camera_optimizer_state optimize_once(bvxm_voxel_world_sptr const& vox_world,
                                           vil_image_view_base_sptr &prev_img, vil_image_view<float> &prev_mask,
                                           bvxm_image_metadata &curr_img, breg3d_ekf_camera_optimizer_state &prev_state,
                                           bool use_gps);

 private:
  bool use_gps_;
  bool use_proj_homography_;
  bool use_expected_;

  vnl_matrix<double> prediction_error_covar_;
  vnl_matrix<double> measurement_error_covar_;

  vnl_vector<double> img_homography(vil_image_view_base_sptr &base_img_viewb, vil_image_view_base_sptr &img_viewb, vil_image_view<float> &mask_view,
                                    vnl_matrix_fixed<double,3,3> K, bool projective = false);

  vnl_vector_fixed<double,6> matrix_to_coeffs_SE3(vnl_matrix_fixed<double,4,4> const& M);
  vnl_vector_fixed<double,6> matrix_to_coeffs_GA2(vnl_matrix_fixed<double,3,3> const& M);
  vnl_vector_fixed<double,8> matrix_to_coeffs_P2(vnl_matrix_fixed<double,3,3> const& M);

  vnl_matrix_fixed<double,4,4> coeffs_to_matrix_SE3(vnl_vector_fixed<double,6> const& a);
  vnl_matrix_fixed<double,3,3> coeffs_to_matrix_GA2(vnl_vector_fixed<double,6> const& a);
  vnl_matrix_fixed<double,3,3> coeffs_to_matrix_P2(vnl_vector_fixed<double,8> const &a);

  bool logm_approx(vnl_matrix<double> const& A, vnl_matrix<double> &logA, double tol = 1e-12);

  vnl_matrix<double> SE3_to_H_Jacobian(double plane_theta, double plane_phi, double plane_dz);
  vnl_matrix<double> H_to_SE3_Jacobian(double plane_theta, double plane_phi, double plane_dz);

  breg3d_homography_generator *homography_gen_;
};


#endif
