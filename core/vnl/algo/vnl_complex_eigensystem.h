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
// respectively. The compute eigenvalues are stored in the member 'W'.
//  Computed right eigenvectors are stored in the columns of the
// member 'R' and computed left eigenvectors are stored in the columns
// of the member 'L'. L, R are set to zero when eigenvectors are not
// requested.
//
// The ith right eigenvector v satisfies A*v = W[i]*v
// The ith right eigenvector u satisfies u*A = W[i]*u (no conjugation)
//

//: computes the eigenvalues [+eigenvectors] of a general complex matrix.

class vnl_complex_eigensystem {
public:

  vnl_complex_eigensystem(const vnl_matrix<double> &A_real,const vnl_matrix<double> &A_imag,
		     bool right=true,bool left=false);

  vnl_complex_eigensystem(const vnl_matrix<vnl_double_complex> &A,
		     bool right=true,bool left=false);

  ~vnl_complex_eigensystem();

  int const N;
  vnl_matrix<vnl_double_complex> L; // left evecs
  vnl_matrix<vnl_double_complex> R; // right evecs
  vnl_vector<vnl_double_complex> W; // evals

private:
  void compute(const vnl_matrix<vnl_double_complex> &,bool,bool);
};

#endif
