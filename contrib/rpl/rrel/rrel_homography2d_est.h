#ifndef rrel_homography2d_est_h_
#define rrel_homography2d_est_h_

//:
// \file
// \author Chuck Stewart
// \date March 2001
// \brief Class to maintain data and optimization model for 2d homography estimation

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>

#include <rrel/rrel_estimation_problem.h>

//: Class to maintain data and optimization model for 2d homography estimation.
//  This class assumes each point has a unique correspondence, even
//  though it may be incorrect.   This is the usual assumption used in
//  2d homography estimation.  It probably isn't the best thing to do
//  in practice, though, because correspondences are hard to find
//  without knowing the transformation and robust estimation can pick
//  out the correct correspondences even when they aren't unique.
//
//  The corresponding data points are provided as a vectors of
//  vgl_homg_point_2d.  Corresponding points are assumed to share the
//  same index in the two vectors.
//
//  Several aspects of this class aren't quite up with the "best"
//  techniques in the literature, although the practical significance
//  of this is known to be quite limited.  First, the symmetric
//  transfer error is used in computing residuals.  Second, the
//  weighted least-squares fit is just a robust version of Hartley's
//  normalized 8-point algorithm.  More sophisticated versions could
//  be developed, but this class was written mostly for demonstration
//  purposes.


class rrel_homography2d_est : public rrel_estimation_problem
{
 public:

  //: Constructor from vgl_homg_point_2d's
  //  By default, we want a full 8-DOF homography
  rrel_homography2d_est( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                         const vcl_vector< vgl_homg_point_2d<double> > & to_pts,
                         unsigned int homog_dof = 8 );

  //: Constructor from vnl_vectors
  //  By default, we want a full 8-DOF homography
  rrel_homography2d_est( const vcl_vector< vnl_vector<double> > & from_pts,
                         const vcl_vector< vnl_vector<double> > & to_pts,
                         unsigned int homog_dof = 8 );

  //: Destructor.
  virtual ~rrel_homography2d_est();

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

  //: Convert a homography to a linear parameter list (for estimation).
  //  Overloaded for specialized reduced-DOF homographies (i.e. affine)
  virtual void  homog_to_params(const vnl_matrix<double>&  m,
                                vnl_vector<double>&        p) const;

  //: Convert a linear parameter list (from estimation) to a homography.
  //  Overloaded for specialized reduced-DOF homographies (i.e. affine)
  virtual void  params_to_homog(const vnl_vector<double>&  p,
                                vnl_matrix<double>&        m) const;

 public:  // testing / debugging utility
  //: \brief Print information as a test utility.
  void print_points() const;

 protected:
  void normalize( const vcl_vector< vnl_vector<double> >& pts,
                  const vcl_vector< double >& wgts,
                  vcl_vector< vnl_vector<double> > & norm_pts,
                  vnl_matrix< double > & norm_matrix ) const;

 protected:
  vcl_vector< vnl_vector<double> > from_pts_;
  vcl_vector< vnl_vector<double> > to_pts_;
  int homog_dof_;
  unsigned int min_num_pts_;
};

#endif // rrel_homography2d_est_h_
