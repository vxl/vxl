#ifndef rrel_orthogonal_regression_h_
#define rrel_orthogonal_regression_h_

//:
// \file
// \author Gehua Yang
// \date October 2001
// Class to maintain data and optimization model for orthogonal regression problems.

#include <rrel/rrel_estimation_problem.h>
#include <vnl/vnl_matrix.h>

//: Maintain data and optimization model for orthogonal regression problems.
//  The orthogonal regression problem is to try to solve
//  \f[
//       X p = 0
//  \f]
//  where \f$ X \f$ is a data matrix and \f$ p \f$ is a parameter
//  vector, under the constraint \f$ ||p||=1 \f$.
//
//  Each sample is a vnl_vector. Denoting the length of the vector by
//  m, the length of parameter vector is also m. Since there is one
//  more constraint \f$ ||p||=1 \f$, the degrees of freedom in the
//  parameters is actually m-1.

class rrel_orthogonal_regression : public rrel_estimation_problem {
public:

  //: Constructor from a matrix.
  rrel_orthogonal_regression( const vnl_matrix<double>& pts );

  //: Constructor from a vector of points.
  rrel_orthogonal_regression( const vcl_vector<vnl_vector<double> >& pts );

  //: Destructor.
  virtual ~rrel_orthogonal_regression();

  //: Total number of samples
  unsigned int num_samples( ) const; 

  //: Generate a parameter estimate from a minimal sample set.
  bool fit_from_minimal_set( const vcl_vector<int>& point_indices,
                             vnl_vector<double>& params ) const;

  //: Compute signed fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          vcl_vector<double>& residuals ) const;

  //: Weighted least squares parameter estimate.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& cofact,
                                   const vcl_vector<double>* weights=0 ) const;

public:  // testing / debugging utility
    //: \brief Print information as a test utility.
  void print_points() const;

protected:
  vnl_matrix<double> vars_; 
};
  
#endif
