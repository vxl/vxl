#ifndef vpgl_affine_fm_robust_est_h_
#define vpgl_affine_fm_robust_est_h_

//:
// \file
// \author J.L. Mundy
// \date Dec 2022
// helper class to maintain data and optimization model for estimating
// the affine fundamental matrix using ransac with vrel robust estimation
// Correspondences in relation to Fa are defined by
//     $pl^t[Fa]pr = 0$
//
// see vpgl_affine_fm_compute_5_point for the actual ransac computation
//
#include <iostream>
#include <vector>
#include <vrel/vrel_estimation_problem.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vpgl_affine_fm_robust_est : public vrel_estimation_problem
{
public:
  //  Euclidan points since affine doesn't require projective space
  vpgl_affine_fm_robust_est(const std::vector<vgl_point_2d<double>> & pr_pts,
                            const std::vector<vgl_point_2d<double>> & pl_pts,
                            unsigned int dof = 5);

  //: Destructor.
  ~vpgl_affine_fm_robust_est() override;

  //: Total number of data points.
  unsigned int
  num_samples() const override;

  //: minimum number of points for linear solution
  unsigned int
  min_num_points() const
  {
    return min_num_pts_;
  }

  //: Generate a parameter estimate from a minimal sample set.
  // point indices access any minimal point subset of the input data
  bool
  fit_from_minimal_set(const std::vector<int> & point_indices, vnl_vector<double> & params) const override;

  //: Compute signed fit residuals relative to the parameter estimate.
  void
  compute_residuals(const vnl_vector<double> & params, std::vector<double> & residuals) const override;

  //: \brief Weighted least squares parameter estimate.
  bool
  weighted_least_squares_fit(vnl_vector<double> & params,
                             vnl_matrix<double> & norm_covar,
                             const std::vector<double> * weights = nullptr) const override;

  void
  fill_Fa_from_params(vnl_vector<double> & params)
  {
    Fa_.set_from_params(params[0], params[1], params[2], params[3], params[4]);
  }

  const vpgl_affine_fundamental_matrix<double>
  Fa()
  {
    return Fa_;
  }

protected:
  std::vector<vnl_vector<double>> right_pts_;
  std::vector<vnl_vector<double>> left_pts_;

  unsigned affine_fmatrix_dof_;
  unsigned min_num_pts_;
  unsigned num_samples_;
  vpgl_affine_fundamental_matrix<double> Fa_;
};

#endif // vpgl_affine_fm_robust_est
