#include <iostream>
#include <utility>

#include <vector>
#include <algorithm>
#include "vpgl_affine_camera_robust_est.h"

#include "vnl/vnl_matrix.h"
#include "vnl/vnl_vector.h"
#include <vnl/algo/vnl_svd.h>
#include "vgl/vgl_point_2d.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

vpgl_affine_camera_robust_est::vpgl_affine_camera_robust_est(const std::vector<vgl_point_3d<double>> & from_pts,
                                                             const std::vector<vgl_point_2d<double>> & to_pts)
  : vrel_estimation_problem(8, 4 /*points to instantiate*/)
{
  assert(from_pts.size() == to_pts.size());

  const unsigned n = from_pts.size();
  // convert from vector to vnl_vector type
  vnl_vector<double> pt3d(3), pt2d(2);
  from_pts_.resize(n);
  to_pts_.resize(n);
  for (unsigned int i = 0; i < n; ++i)
  {
    pt3d[0] = from_pts[i].x();
    pt3d[1] = from_pts[i].y();
    pt3d[2] = from_pts[i].z();
    from_pts_[i] = pt3d;
    pt2d[0] = to_pts[i].x();
    pt2d[1] = to_pts[i].y();
    to_pts_[i] = pt2d;
  }

  affine_camera_dof_ = 8;
  min_num_pts_ = 4;
  num_samples_ = n;
}

vpgl_affine_camera_robust_est::vpgl_affine_camera_robust_est(const std::vector<vnl_vector<double>> & from_pts,
                                                             std::vector<vnl_vector<double>> to_pts)
  : vrel_estimation_problem(8 /*dof*/, 4 /*points to instantiate*/)
  , from_pts_(from_pts)
  , to_pts_(std::move(to_pts))
{
  assert(from_pts_.size() == to_pts_.size());
  const unsigned size = from_pts.size();

  affine_camera_dof_ = 8;
  min_num_pts_ = 4;
  num_samples_ = size;
}

vpgl_affine_camera_robust_est::~vpgl_affine_camera_robust_est() = default;


unsigned int
vpgl_affine_camera_robust_est::num_samples() const
{
  return num_samples_;
}

//  The equation to be solved is A p = b, where A is a dof_ x dof_
//  array formed from the independent variables and b is dof_ x 1 and
//  formed from the dependent variables.  If A is not full-rank, false
//  is returned.  Otherwise, params = A^{-1} b and true is returned.
//
bool
vpgl_affine_camera_robust_est::fit_from_minimal_set(const std::vector<int> & point_indices,
                                                    vnl_vector<double> & params) const
{
  if (point_indices.size() != min_num_pts_)
  {
    std::cerr << "vpgl_affine_camera_robust_est::fit_from_minimal_sample  The number of point "
              << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }
  vnl_matrix<double> A(8, 8, 0.0);
  vnl_vector<double> b(8);
  for (unsigned int i = 0; i < min_num_pts_; ++i)
  {
    unsigned int indx = point_indices[i];
    A[2 * i][0] = from_pts_[indx][0];
    A[2 * i][1] = from_pts_[indx][1];
    A[2 * i][2] = from_pts_[indx][2];
    A[2 * i][3] = 1;
    b[2 * i] = to_pts_[indx][0];
    A[2 * i + 1][4] = from_pts_[indx][0];
    A[2 * i + 1][5] = from_pts_[indx][1];
    A[2 * i + 1][6] = from_pts_[indx][2];
    A[2 * i][7] = 1;
    b[2 * i + 1] = to_pts_[indx][1];
  }
  vnl_svd<double> svd(A, 1.0e-8);
  if ((unsigned int)svd.rank() < min_num_pts_)
  {
    return false; // singular fit --- no error message needed
  }
  else
  {
    params = svd.solve(b);
  }
  return true;
}

void
vpgl_affine_camera_robust_est::compute_residuals(const vnl_vector<double> & params,
                                                 std::vector<double> & residuals) const
{
  assert(residuals.size() == num_samples_);
  for (unsigned int i = 0; i < num_samples_; ++i)
  {
    double u = params[0] * from_pts_[i][0] + params[1] * from_pts_[i][1] + params[2] * from_pts_[i][2] + params[3];
    double v = params[4] * from_pts_[i][0] + params[5] * from_pts_[i][1] + params[6] * from_pts_[i][2] + params[7];
    double ut = to_pts_[i][0], vt = to_pts_[i][1];
    residuals[i] = sqrt((ut - u) * (ut - u) + (vt - v) * (vt - v));
  }
}


bool
vpgl_affine_camera_robust_est::weighted_least_squares_fit(vnl_vector<double> & params,
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
  vnl_matrix<double> A(2 * num_samples_, 8, 0.0);
  vnl_vector<double> b(2 * num_samples_);
  // if mapping confidence is trustworthy maybe use as weights
  for (size_t i = 0; i < num_samples_; ++i)
  {
    A[2 * i][0] = from_pts_[i][0];
    A[2 * i][1] = from_pts_[i][1];
    A[2 * i][2] = from_pts_[i][2];
    A[2 * i][3] = 1;
    b[2 * i] = to_pts_[i][0];
    A[2 * i + 1][4] = from_pts_[i][0];
    A[2 * i + 1][5] = from_pts_[i][1];
    A[2 * i + 1][6] = from_pts_[i][2];
    A[2 * i][7] = 1;
    b[2 * i + 1] = to_pts_[i][1];
  }

  vnl_svd<double> svd(A, 1.0e-8);
  if ((unsigned int)svd.rank() < params.size())
  {
    return false; // singular fit --- no error message needed
  }
  else
  {
    params = svd.solve(b);
  }
  return true;
}
