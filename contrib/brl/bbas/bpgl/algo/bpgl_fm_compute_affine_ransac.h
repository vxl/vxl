// This is bbas/bpgl/algo/bpgl_fm_compute_affine_ransac.h
#ifndef bpgl_fm_compute_affine_ransac_h_
#define bpgl_fm_compute_affine_ransac_h_
//:
// \file
// \brief Compute the fundamental matrix from rectified image correspondences.
//
// A robust algorithm for computing the fundamental matrix from lists of
// corresponding points between two rectified images.
// This uses RREL to do the robust computation.
// \author Thomas Pollard
// \date May 08, 2005
//
// Should template this class.

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <rrel/rrel_estimation_problem.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>

class bpgl_fm_compute_affine_ransac_params;


//: This is the main class for computing the fundamental matrix from lists of corresponding points.
class bpgl_fm_compute_affine_ransac
{
 public:
  bpgl_fm_compute_affine_ransac( bpgl_fm_compute_affine_ransac_params* params){
    params_ = params; }

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  bool compute( const std::vector< vgl_point_2d<double> >& pr,
                const std::vector< vgl_point_2d<double> >& pl,
                vpgl_affine_fundamental_matrix<double>& fm );

  //: After "compute" this will have true in the indices determined to be outliers.
  std::vector<bool> outliers;

 protected:
  bpgl_fm_compute_affine_ransac_params* params_;
};


//: Class with parameters for the above class.
class bpgl_fm_compute_affine_ransac_params
{
 public:
  bpgl_fm_compute_affine_ransac_params();

  double max_outlier_frac;
  double desired_prob_good;
  int max_pops;
  int trace_level;
  double residual_thresh;
};


//: This is a helper class for bpgl_fm_compute_ransac using rrel.
class rrel_fm_affine_problem : public rrel_estimation_problem
{
 public:
  //: Construct the problem object with two sets of corresponding points.
  // Points pr correspond to the RHS of the fundamental matrix, while the
  // points pl correspond to the LHS.
  rrel_fm_affine_problem( const std::vector< vgl_point_2d<double> > & pr,
                          const std::vector< vgl_point_2d<double> > & pl );

  ~rrel_fm_affine_problem() override = default;

  // Total number of correspondences.
  unsigned int num_samples() const override{ return pr_.size(); }

  // The degrees of freedom in the residual.
  unsigned int residual_dof() const override { return 4; }//NOT SURE WHAT THIS SHOULD BE

  // Generate a parameter estimate from a minimal sample.
  bool fit_from_minimal_set( const std::vector<int>& point_indices,
                             vnl_vector<double>& params ) const override;

  // Compute unsigned fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          std::vector<double>& residuals ) const override;

  // Convert a fundamental matrix into a parameter vector.
  virtual void  fm_to_params( const vpgl_affine_fundamental_matrix<double>&  fm,
                              vnl_vector<double>& p) const;

  // Convert a parameter vector into a fundamental matrix.
  virtual void  params_to_fm( const vnl_vector<double>& p,
                              vpgl_affine_fundamental_matrix<double>& fm) const;

  //: Weighted least squares parameter estimate.
  //  The normalized covariance is not yet filled in.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const std::vector<double>* weights=nullptr ) const override;

  // Toggles detailed printing of computations.
  bool verbose;

 protected:
  std::vector< vgl_point_2d<double> > pr_;
  std::vector< vgl_point_2d<double> > pl_;
};

#endif // bpgl_fm_compute_affine_ransac_h_
