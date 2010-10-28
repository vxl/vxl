// This is algo/bapl/bapl_affine2d_est_h_
#ifndef bapl_affine2d_est_h_
#define bapl_affine2d_est_h_

//:
// \file
// \brief Class to maintain data and optimization model for 2d affine transform estimation
// \author Matthew Leotta (mleotta@lems.brown.edu)
// \date March 2001
//
// This code is a simplified version of rrel_homography2d_est by Chuck Stewart
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vnl/vnl_fwd.h>
#include <bapl/bapl_lowe_cluster.h>

#include <rrel/rrel_estimation_problem.h>

//: Class to maintain data and optimization model for 2d affine transform estimation.

class bapl_affine2d_est : public rrel_estimation_problem {
public:

  //: Constructor from a vector of matches (pairs of keypoint smart pointers)
  bapl_affine2d_est( const vcl_vector< bapl_keypoint_match > & matches );

  //: Constructor from vnl_vectors
  bapl_affine2d_est( const vcl_vector< vnl_vector<double> > & from_pts,
                         const vcl_vector< vnl_vector<double> > & to_pts );

  //: Destructor.
  virtual ~bapl_affine2d_est();

  //: Total number of correspondences.
  unsigned int num_samples( ) const;

  //: The degrees of freedom in the residual.
  // Each coordinate of the correspondence pair has Gaussian error, so
  // the Euclidean distance residual has 4 degrees of freedom.
  unsigned int residual_dof() const { return 4; }

  //: Generate a parameter estimate from a minimal sample.
  bool fit_from_minimal_set( const vcl_vector<int>& point_indices,
                             vnl_vector<double>& params ) const;

  //: Compute unsigned fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          vcl_vector<double>& residuals ) const;

  //: Weighted least squares parameter estimate.  The normalized covariance is not yet filled in.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const vcl_vector<double>* weights=0 ) const;

public:  // testing / debugging utility
    //: \brief Print information as a test utility.
  void print_points() const;

protected:
  void normalize( const vcl_vector< vnl_vector<double> >& pts,
                  const vcl_vector< double >& wgts,
                  vcl_vector< vnl_vector<double> > & norm_pts,
                  vnl_vector< double > & center,
                  double &avg_distance ) const;

protected:
  vcl_vector< vnl_vector< double > > from_pts_;
  vcl_vector< vnl_vector< double > > to_pts_;
};

#endif // bapl_affine2d_est_h_
