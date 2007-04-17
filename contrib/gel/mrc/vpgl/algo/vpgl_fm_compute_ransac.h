// This is gel/mrc/vpgl/algo/vpgl_fm_compute_ransac.h
#ifndef vpgl_fm_compute_ransac_h_
#define vpgl_fm_compute_ransac_h_
//:
// \file
// \brief Compute the fundamental matrix from image correspondences.
//
// A robust algorithm for computing the fundamental matrix from lists
// of corresponding points.  This uses RREL to do the robust computation.
// \author Thomas Pollard
// \date 5/27/05
//
//  The point correspondences in relation to F are defined by
//     pl^t[F]pr = 0
//
// Should template this class.

#include <vcl_vector.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <rrel/rrel_estimation_problem.h>
#include <vpgl/vpgl_fundamental_matrix.h>

//: This is the main class for computing the fundamental matrix from lists of corresponding points.
class vpgl_fm_compute_ransac
{
 public:
  vpgl_fm_compute_ransac():outlier_thresh_(1),max_outlier_frac_(0.5),
    desired_prob_good_(0.99), max_pops_(1), gen_all_(false), trace_level_(0) {}

  //: Compute from two sets of corresponding points.
  // Put the resulting matrix into fm, return true if successful.
  // Points pr are associated with the RHS of the fundamental matrix
  // while the points pl are associated with the LHS.
  bool compute( const vcl_vector< vgl_point_2d<double> >& pr,
                const vcl_vector< vgl_point_2d<double> >& pl,
                vpgl_fundamental_matrix<double>& fm );

  //: The upper bound on the fraction of outlier correspondences
  void set_max_outlier_frac(const double max_frac){max_outlier_frac_ = max_frac;}
  //: The probability that a correct correspondence tuple is found
  void set_desired_prob_good(const double prob_good){desired_prob_good_ = prob_good;}
  //: the max number of populations in the sample (typically one)
  void set_max_pops(const int max_pops){max_pops_=max_pops;}

  //: Force the generation of all sample 8 tuples
  void set_generate_all(const bool gen_all){gen_all_ = gen_all;}

  //: Set the threshold on epipolar distance that determines that a correspondence is an outlier
  void set_outlier_threshold(const double thresh){outlier_thresh_ = thresh;}

  //: Set the trace level for debugging
  void set_trace_level(int trace_level) { trace_level_ = trace_level; }
  
  //: After "compute" indices will have true set for correspondences that are outliers
  vcl_vector<bool> outliers;

  //: After "compute" this will have point distances from epipolar lines
  vcl_vector<double> residuals;

 private:
  double outlier_thresh_;
  double max_outlier_frac_;
  double desired_prob_good_;
  int max_pops_; 
  bool gen_all_;
  int trace_level_;
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

  //: Weighted least squares parameter estimate.
  //  The normalized covariance is not yet filled in.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
    vnl_matrix<double>& norm_covar, const vcl_vector<double>* weights=0 ) const;

  // Toggles detailed printing of computations.
  bool verbose;

 protected:
  vcl_vector< vgl_point_2d<double> > pr_;
  vcl_vector< vgl_point_2d<double> > pl_;
};

#endif // vpgl_fm_compute_ransac_h_
