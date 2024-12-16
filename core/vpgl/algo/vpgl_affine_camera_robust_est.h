#ifndef vpgl_affine_camera_robust_est_h_
#define vpgl_affine_camera_robust_est_h_

//:
// \file
// \author J.L. Mundy
// \date Dec 31, 2022
// \brief Class to maintain data and optimization model for robust estimation of  affine camera parameters
//
#include <iostream>
#include <vector>
#include <vrel/vrel_estimation_problem.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vpgl/vpgl_affine_camera.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vpgl_affine_camera_robust_est : public vrel_estimation_problem
{
public:
  //: constructor with vgl points - projection from 3-d to 2-d
  vpgl_affine_camera_robust_est(const std::vector<vgl_point_3d<double>> & from_pts,
                                const std::vector<vgl_point_2d<double>> & to_pts);

  //: Constructor with vnl vectors - projection from 3-d to 2-d
  vpgl_affine_camera_robust_est(const std::vector<vnl_vector<double>> & from_pts,
                                std::vector<vnl_vector<double>> to_pts);

  //: Destructor.
  ~vpgl_affine_camera_robust_est() override;

  //: Total number of data points.
  unsigned int
  num_samples() const override;


  //: Generate a parameter estimate from a minimal sample set.
  bool
  fit_from_minimal_set(const std::vector<int> & point_indices, vnl_vector<double> & params) const override;

  //: Compute residuals relative to the parameter estimate.
  void
  compute_residuals(const vnl_vector<double> & params, std::vector<double> & residuals) const override;

  //: \brief Weighted least squares parameter estimate.
  bool
  weighted_least_squares_fit(vnl_vector<double> & params,
                             vnl_matrix<double> & norm_covar,
                             const std::vector<double> * weights = nullptr) const override;

  void
  fill_camera_from_params(const vnl_vector<double> & params)
  {
    vnl_matrix_fixed<double, 3, 4> Ma(0.0);
    Ma[0][0] = params[0];
    Ma[0][1] = params[1];
    Ma[0][2] = params[2];
    Ma[0][3] = params[3];
    Ma[1][0] = params[4];
    Ma[1][1] = params[5];
    Ma[1][2] = params[6];
    Ma[1][3] = params[7];
    Ma[2][3] = 1.0;
    Ca_.set_matrix(Ma);
  }
  //: The robust estimate
  const vpgl_affine_camera<double>
  Ca()
  {
    return Ca_;
  }

protected:
  std::vector<vnl_vector<double>> from_pts_; // 3-d points
  std::vector<vnl_vector<double>> to_pts_;   // 2-d points

  unsigned affine_camera_dof_;
  unsigned min_num_pts_;
  unsigned num_samples_;
  vpgl_affine_camera<double> Ca_;
};

#endif // vpgl_affine_camera_robust_est
