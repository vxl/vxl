//:
// \file
// \author Tim Cootes
// \brief Error term for calculation of correction to projective matrix

#ifndef m2d3_correction_matrix_error_h_
#define m2d3_correction_matrix_error_h_

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_least_squares_function.h>

//: Computes error on constraints when computing correction matrix
//  Given g, returns error vector (Aq-rhs), where vector q are the
//  unique elements of the symmetric matrix Q=GG', G being the
//  3(m+1) x 3 matrix formed from the elements of the supplied
//  vector g.
class m23d_correction_matrix_error : public vnl_least_squares_function
{
  //: Workspace for unique elements of symmetric matrix GG'
  vnl_vector<double> q_;

  //: Constraints on q
  vnl_matrix<double> A_;

  //: RHS for constraints on q  (Aq=rhs)
  vnl_vector<double> rhs_;

  //: Workspace for residual vector
  vnl_vector<double> r_;

  //: Number of model modes
  unsigned n_modes_;

  //: Current basis of interest
  unsigned k_;

  //: Compute q from supplied g
  //  g1==g2 for full q, g2 = (0 0 ..1 ...0) when computing derivatives
  void compute_q(const vnl_vector<double>& g1,
                 const vnl_vector<double>& g2,
                 vnl_vector<double>& q);

public:
  m23d_correction_matrix_error(const vnl_matrix<double>& A,
                                 const vnl_vector<double>& rhs,
                                 unsigned n_modes,
                                 unsigned k);

  //: The main function.
  //  Given g, returns error vector fx=(Aq-rhs), where vector q are the
  //  unique elements of the symmetric matrix Q=GG', G being the
  //  3(m+1) x 3 matrix formed from the elements of the supplied
  //  vector g.
  void f(vnl_vector<double> const& g,
                 vnl_vector<double>& fx) override;

  //: Calculate the Jacobian, given the parameter vector g.
  void gradf(vnl_vector<double> const& g,
                     vnl_matrix<double>& jacobian) override;

};


#endif // m2d3_correction_matrix_error_h_
