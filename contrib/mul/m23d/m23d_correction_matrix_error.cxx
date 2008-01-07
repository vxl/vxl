#include "m23d_correction_matrix_error.h"
//:
// \file
// \author Tim Cootes
// \brief Error term for calculation of correction to projective matrix

#include <mbl/mbl_matxvec.h>
#include <vcl_cassert.h>

m23d_correction_matrix_error::m23d_correction_matrix_error(const vnl_matrix<double>& A,
                              const vnl_vector<double>& rhs,
                              unsigned n_modes,
                              unsigned k)
  : vnl_least_squares_function(9*(n_modes+1),A.rows(),use_gradient),
    A_(A),rhs_(rhs),n_modes_(n_modes),k_(k)
{
  unsigned t = 3*(n_modes+1);
  assert(A.cols() == (t*(t+1))/2);
  q_.set_size(A.cols());
}

//: Compute q from supplied g
//  g1==g2 for full q, g2 = (0 0 ..1 ...0) when computing derivatives
//  q is set to the unique elements of G1.G2', where G1 and G2 are the
//  3*(m+1) x 3 matrices formed from the elements of g1 and g2.
void m23d_correction_matrix_error::compute_q(const vnl_vector<double>& g1,
                 const vnl_vector<double>& g2,
                 vnl_vector<double>& q)
{
  unsigned t = 3*(n_modes_+1);
  unsigned k=0;
  const double* r1=g1.data_block();
  for (unsigned i=0;i<t;++i,r1+=3)
  {
    const double* r2=g2.data_block();
    for (unsigned j=0;j<=i;++j,++k,r2+=3)
       q[k]= r1[0]*r2[0] + r1[1]*r2[1] +r1[2]*r2[2];
  }
}


//: The main function.
//  Given g, returns error vector fx=(Aq-rhs), where vector q are the
//  unique elements of the symmetric matrix Q=GG', G being the
//  3(m+1) x 3 matrix formed from the elements of the supplied
//  vector g.
void m23d_correction_matrix_error::f(vnl_vector<double> const& g,
                                     vnl_vector<double>& fx)
{
  assert(g.size() == 9*(n_modes_+1));
  compute_q(g,g,q_);
  mbl_matxvec_prod_mv(A_,q_,fx);
  fx-=rhs_;
}

//: Calculate the Jacobian, given the parameter vector g.
void m23d_correction_matrix_error::gradf(vnl_vector<double> const& g,
                                         vnl_matrix<double>& jacobian)
{
  unsigned ng = 9*(n_modes_+1);
  assert(g.size() == ng);
  jacobian.set_size(get_number_of_residuals(),ng);
  r_.set_size(get_number_of_residuals());

  vnl_vector<double> g1(ng,0.0),q1(A_.cols());
  for (unsigned j=0;j<ng;++j)
  {
    g1[j]=1.0;
    compute_q(g,g1,q_);
    compute_q(g1,g,q1);
    q_+=q1;
    mbl_matxvec_prod_mv(A_,q_,r_);
    jacobian.set_column(j,r_);
    g1[j]=0.0;  // Reset to zero
  }
}
