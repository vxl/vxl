#include <iostream>
#include <utility>

#include <vector>
#include <algorithm>
#include "vpgl_affine_fm_robust_est.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <vnl/algo/vnl_svd.h>
#include "vgl/vgl_point_2d.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

vpgl_affine_fm_robust_est::vpgl_affine_fm_robust_est(const std::vector<vgl_point_2d<double>> & pr_pts,
                                                     const std::vector<vgl_point_2d<double>> & pl_pts,
                                                     unsigned dof)
  : vrel_estimation_problem(dof, dof /*points to instantiate*/)
{
  assert(pr_pts.size() == pl_pts.size());


  const unsigned size = pr_pts.size();

  // convert from vector to vnl_vector type
  vnl_vector<double> pt(2);
  right_pts_.resize(size);
  left_pts_.resize(size);
  for (unsigned int i = 0; i < size; ++i)
  {
    pt[0] = pr_pts[i].x();
    pt[1] = pr_pts[i].y();
    right_pts_[i] = pt;
    pt[0] = pl_pts[i].x();
    pt[1] = pl_pts[i].y();
    left_pts_[i] = pt;
  }

  affine_fmatrix_dof_ = dof;
  min_num_pts_ = dof;
  num_samples_ = size;
}

vpgl_affine_fm_robust_est::~vpgl_affine_fm_robust_est() = default;


unsigned int
vpgl_affine_fm_robust_est::num_samples() const
{
  return num_samples_;
}

//  The equation to be solved is A p = b, where A is a dof_ x dof_
//  array formed from the independent variables and b is dof_ x 1 and
//  formed from the dependent variables.  If A is not full-rank, false
//  is returned.  Otherwise, params = A^{-1} b and true is returned.
//
bool
vpgl_affine_fm_robust_est::fit_from_minimal_set(const std::vector<int> & point_indices,
                                                vnl_vector<double> & params) const
{
  if (point_indices.size() != min_num_pts_)
  {
    std::cerr << "vpgl_affine_fm_robust_est::fit_from_minimal_sample  The number of point "
              << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }
  vnl_matrix<double> A(5, 5, 0.0);
  for (unsigned int i = 0; i < min_num_pts_; ++i)
  {
    unsigned int indx = point_indices[i];
    A[i][0] = left_pts_[indx][0];
    A[i][1] = left_pts_[indx][1];
    A[i][2] = right_pts_[indx][0];
    A[i][3] = right_pts_[indx][1];
    A[i][4] = 1.0;
  }
  vnl_svd<double> svd(A, 1.0e-8);
  // std::cout << svd.W() << std::endl;
  if ((unsigned int)svd.rank() < min_num_pts_ - 1)
  {
    return false; // singular fit --- no error message needed
  }
  else
  {
    params = svd.nullvector();
  }
  return true;
}

void
vpgl_affine_fm_robust_est::compute_residuals(const vnl_vector<double> & params, std::vector<double> & residuals) const
{
  assert(residuals.size() == num_samples_);
  double a = params[0], b = params[1];
  double s = sqrt(a * a + b * b);
  vnl_vector<double> scaled_params = params / s;
  double epi_error;
  for (unsigned int i = 0; i < num_samples_; ++i)
  {
    epi_error = scaled_params[0] * left_pts_[i][0] + scaled_params[1] * left_pts_[i][1] +
                scaled_params[2] * right_pts_[i][0] + scaled_params[3] * right_pts_[i][1] + scaled_params[4];
    residuals[i] = fabs(epi_error);
  }
}


bool
vpgl_affine_fm_robust_est::weighted_least_squares_fit(vnl_vector<double> & params,
                                                      vnl_matrix<double> & norm_covar,
                                                      const std::vector<double> * weights) const
{
  std::vector<double> tmp_wgts;
  if (!weights)
  {
    // set weight to one
    tmp_wgts.resize(num_samples_);
    std::fill(tmp_wgts.begin(), tmp_wgts.end(), 1.0);
    weights = &tmp_wgts;
  }
  vnl_matrix<double> A(num_samples_, 5, 0.0);
  // if mapping confidence is trustworthy maybe use as weights
  for (size_t i = 0; i < num_samples_; ++i)
  {
    A[i][0] = left_pts_[i][0];
    A[i][1] = left_pts_[i][1];
    A[i][2] = right_pts_[i][0];
    A[i][3] = right_pts_[i][1];
    A[i][4] = 1.0;
  }

  vnl_svd<double> svd(A, 1.0e-8);
  if ((unsigned int)svd.rank() < min_num_pts_)
  {
    return false; // singular fit --- no error message needed
  }
  else
  {
    params = svd.nullvector();
  }
  return true;
}
