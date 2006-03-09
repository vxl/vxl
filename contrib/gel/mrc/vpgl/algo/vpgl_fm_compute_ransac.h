// This is gel/mrc/vpgl/algo/vpgl_fm_compute_ransac.h
#ifndef _vpgl_fm_compute_ransac_h_
#define _vpgl_fm_compute_ransac_h_
//:
// \file
// \brief A robust algorithm for computing the fundamental matrix from lists
// of corresponding points.  This uses RREL to do the robust computation.
// \author Thomas Pollard
// \date 5/27/05
//
// Should template this class.

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <rrel/rrel_estimation_problem.h>
#include <vpgl/vpgl_fundamental_matrix.h>

//: This is the main class for computing the fundamental matrix from lists of
// corresponding points.
class vpgl_fm_compute_ransac
{
public:

  vpgl_fm_compute_ransac(){};

  //: Compute from two sets of corresponding points, put the resulting matrix
  // into fm, return true if successful.  Points pr are associated with the RHS
  // of the fundamental matrix while the points pl are associated with the LHS.
  bool compute( const vcl_vector< vgl_point_2d<double> >& pr,
                const vcl_vector< vgl_point_2d<double> >& pl,
                vpgl_fundamental_matrix<double>& fm );

  //: After "compute" this will have true in the indices determined to be outliers.
  vcl_vector<bool> outliers;
};


//: This is a helper class for vpgl_fm_compute_ransac using rrel.
class rrel_fm_problem : public rrel_estimation_problem 
{

public:

  //: Construct the problem object with two sets of corresponding points.
  // Points pr correspond to the RHS of the fundamental matrix, while the
  // points pl correspond to the LHS.
  rrel_fm_problem( const vcl_vector< vgl_point_2d<double> > & pr,
                   const vcl_vector< vgl_point_2d<double> > & pl );

  virtual ~rrel_fm_problem(){};

  // Total number of correspondences.
  unsigned int num_samples() const{ return pr_.size(); }

  // The degrees of freedom in the residual.
  unsigned int residual_dof() const { return 4; }

  // Generate a parameter estimate from a minimal sample.
  bool fit_from_minimal_set( const vcl_vector<int>& point_indices,
                             vnl_vector<double>& params ) const;

  // Compute unsigned fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          vcl_vector<double>& residuals ) const;

  // Convert a fundamental matrix into a parameter vector.
  virtual void  fm_to_params( const vpgl_fundamental_matrix<double>&  fm,
                              vnl_vector<double>& p) const;

  // Convert a parameter vector into a fundamental matrix.
  virtual void  params_to_fm( const vnl_vector<double>& p, 
                              vpgl_fundamental_matrix<double>& fm) const;

  //: Weighted least squares parameter estimate.  The normalized covariance 
  // is not yet filled in.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
    vnl_matrix<double>& norm_covar, const vcl_vector<double>* weights=0 ) const;

  // Toggles detailed printing of computations.
  bool verbose;

protected:

  vcl_vector< vgl_point_2d<double> > pr_;
  vcl_vector< vgl_point_2d<double> > pl_;

};

#endif //_vpgl_fm_compute_ransac_h_
