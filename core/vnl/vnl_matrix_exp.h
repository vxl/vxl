#ifndef vnl_matrix_exp_h_
#define vnl_matrix_exp_h_
#ifdef __GNUC__
#pragma interface "vnl_matrix_exp"
#endif
/*
  fsm@robots.ox.ac.uk
*/

// Compute the exponential of a square matrix, by summing its
// exponential series exp(X) = \sum_{n \ge 0} X^n/n! till an
// convergence requirement is met.
//
// Many improvements are possible.

#include <vnl/vnl_matrix.h>

//: fiddly form.
template <class T>
bool vnl_matrix_exp(vnl_matrix<T> const &X, vnl_matrix<T> &expX, double max_err);


//: easy form.
template <class T>
vnl_matrix<T> vnl_matrix_exp(vnl_matrix<T> const &X);

#endif
