//:
// \file
// \author Tim Cootes
// \brief Functions to construct linear constraints as rows in a matrix

#ifndef m23d_set_q_constraint_h_
#define m23d_set_q_constraint_h_

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//: Defines constraint on elements of Q of form a'Qb=r
//  Q is a symmetric matrix, with n(n+1)/2 independent elements
//  The constraints on these elements are placed in row c of A
void m23d_set_q_constraint1(vnl_matrix<double> & A,
                      vnl_vector<double> & rhs,
                      unsigned c,
                      const vnl_vector<double>& a,
                      const vnl_vector<double>& b,
                      double r);

//: Defines constraint on elements of Q of form aQa'-bQb'=0
//  Q is a symmetric matrix, with n(n+1)/2 independent elements
//  The constraints on these elements are placed in row c of A
void m23d_set_q_constraint2(vnl_matrix<double> & A,
                      vnl_vector<double> & rhs,
                      unsigned c,
                      const vnl_vector<double>& a,
                      const vnl_vector<double>& b);

#endif // m23d_set_q_constraint_h_



