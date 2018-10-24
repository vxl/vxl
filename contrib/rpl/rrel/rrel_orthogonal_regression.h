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
  rrel_orthogonal_regression( const std::vector<vnl_vector<double> >& pts );

  //: Destructor.
  ~rrel_orthogonal_regression() override;

  //: Total number of samples
  unsigned int num_samples( ) const override;

  //: Generate a parameter estimate from a minimal sample set.
  bool fit_from_minimal_set( const std::vector<int>& point_indices,
                             vnl_vector<double>& params ) const override;

  //: Compute signed fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          std::vector<double>& residuals ) const override;

  //: Weighted least squares parameter estimate.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& cofact,
                                   const std::vector<double>* weights=nullptr ) const override;

public:  // testing / debugging utility
    //: \brief Print information as a test utility.
  void print_points() const;

protected:
  vnl_matrix<double> vars_;
};

#endif
