#ifndef vnl_complex_eigensystem_h_
#define vnl_complex_eigensystem_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vnl/vnl_complex.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

//
// Class to computes the eigenvalues and (optionally) eigenvectors
// of a general complex matrix. Uses the LAPACK routine zgeev.
//
// Default behaviour is to compute the eigenvalues and the right
// eigenvectors.
//
// The input NxN matrix A is passed into the constructor. The flags
// right,left request the calculation of right and left eigenvectors
// respectively. The compute eigenvalues are stored in the member 'W_'.
//  Computed right eigenvectors are stored in the columns of the
// member 'R_' and computed left eigenvectors are stored in the columns
// of the member 'L_'. L_, R_ are set to zero when eigenvectors are not
// requested.
//
// The ith right eigenvector v satisfies A*v = W_[i]*v
// The ith right eigenvector u satisfies u*A = W_[i]*u (no conjugation)
//

//: computes the eigenvalues [+eigenvectors] of a general complex matrix.

class vnl_complex_eigensystem {
public:

  vnl_complex_eigensystem(const vnl_matrix<double> &A_real,const vnl_matrix<double> &A_imag,
		     bool right=true,bool left=false);

  vnl_complex_eigensystem(const vnl_matrix<vnl_double_complex> &A,
		     bool right=true,bool left=false);

  ~vnl_complex_eigensystem();

  int const N_;
  vnl_matrix<vnl_double_complex> L_; // left evecs
  vnl_matrix<vnl_double_complex> R_; // right evecs
  vnl_vector<vnl_double_complex> W_; // evals

private:
  void compute(const vnl_matrix<vnl_double_complex> &,bool,bool);
};

#endif
