#ifndef rrel_linear_regression_h_
#define rrel_linear_regression_h_

//:
// \file
// \author Chuck Stewart
// \date March 2001
// Class to maintain data and optimization model for single linear regression problems.

#include <rrel/rrel_estimation_problem.h>

//: Class to maintain data and optimization model for single linear regression problems.
//  The linear regression problem is to estimate the parameter vector \f$ a \f$ in
//  \f[
//      y = Xa + \epsilon
//  \f]
//  where \f$ \epsilon \f$ is a zero-mean random error variable.
//
//  Each sample is a vnl_vector.  Denoting the length of the
//  vector by m, the first m-1 terms are assumed to be the independent
//  variables, while the last term is the dependent (random) variable.
//  This is suitable for surface estimation from range data ---
//  planar, quadratics, etc --- where most of the error is in the
//  depth direction.  Assuming a data point vector \f$x = (x_1, ...,
//  x_m)\f$, if the "use_intercept" parameter sent to the constructor is
//  true and then the regression model is
//  \f[
//      x_m = a_1 + a_2 x_1 + ... + a_{m} x_{m-1}
//  \f]
//  Otherwise, the regression model is
//  \f[
//      x_m = a_1 x_1 + ... + a_{m-1} x_{m-1}
//  \f]
//  The parameter vector is \f$a = (a_1, ..., a_{m})\f$ for the former and
//  \f$a = (a_1,...,a_{m-1})\f$ for the latter.
//
//  To illustrate, consider range data, where each measured point is
//  p = (x,y,z).  For planar fits, the input pts to the class
//  constructor are then just p, so the regression model is
//  \f[
//      z = a_0 + a_1 x + a_2 y
//  \f]
//  For quadratic fits, the data points (vnl_vectors provided as input
//  to the constructor) are \f$(x, y, x^2, xy, y^2, z)\f$ and the regression
//  model is
//  \f[
//      z = a_0 + a_1 x + a_2 y + a_3 x^2 + a_4 xy + a_5 y^2
//  \f]

class rrel_linear_regression : public rrel_estimation_problem {
public:
  //: Constructor that includes all information in the sample vectors.
  //  For each sample, the first m-1 entries are the independent
  //  variables, and the last entry is the dependent variable.
  rrel_linear_regression( const std::vector<vnl_vector<double> >& pts,
                          bool use_intercept=true);

  //: Constructor with data pre-separated into arrays of independent and dependent variables.
  rrel_linear_regression( const std::vector< vnl_vector<double> >&  ind_vars,
                          std::vector< double >   dep_vars );

  //: Destructor.
  ~rrel_linear_regression() override;

  //: Total number of data points.
  unsigned int num_samples( ) const override;

  //: Generate a parameter estimate from a minimal sample set.
  bool fit_from_minimal_set( const std::vector<int>& point_indices,
                             vnl_vector<double>& params ) const override;

  //: Compute signed fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          std::vector<double>& residuals ) const override;

  //: \brief Weighted least squares parameter estimate.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const std::vector<double>* weights=nullptr ) const override;

public:  // testing / debugging utility
    //: \brief Print information as a test utility.
  void print_points() const;

protected:
  std::vector<double> rand_vars_;               // e.g. the z or depth values
  std::vector<vnl_vector<double> > ind_vars_;   // e.g. the image coordinates (plus 1.0
                                               // for intercept parameters)
};

#endif
