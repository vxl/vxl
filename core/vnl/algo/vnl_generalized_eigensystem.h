#ifndef vnl_generalized_eigensystem_h_
#define vnl_generalized_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief  Solves the generalized eigenproblem Ax=La
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   29 Aug 96
//
// \verbatim
// Modifications
//  dac (Manchester) 28/03/2001: tidied up documentation
// \endverbatim

#include <vnl/vnl_diag_matrix.h>

//: Solves the generalized eigenproblem Ax=La
//  Solves the generalized eigenproblem of \f$A x = \lambda B x\f$,
//  with \f$A\f$ symmetric and \f$B\f$ positive definite.
//  See Golub and van Loan, Section 8.7.

class vnl_generalized_eigensystem {
public:
// Public data members because they're unique.
  int n;

//: Solve real generalized eigensystem \f$A x = \lambda B x\f$ for
//  \f$\lambda\f$ and \f$x\f$, where \f$A\f$ symmetric, \f$B\f$
//  positive definite.
//  Initializes storage for the matrix \f$V = [ x_0 x_1 .. x_n ]\f$ and
//  the vnl_diag_matrix \f$D = [ \lambda_0 \lambda_1 ... \lambda_n ]\f$.
//  The eigenvalues are sorted into increasing order (of value, not
//  absolute value).
//
//  Uses vnl_cholesky decomposition \f$C^\top C = B\f$, to convert to
//  \f$C^{-\top} A C^{-1} x = \lambda x\f$ and then uses the
//  Symmetric eigensystem code.   It will print a verbose warning
//  if \f$B\f$ is not positive definite.

  vnl_generalized_eigensystem(const vnl_matrix<double>& A,
        const vnl_matrix<double>& B);

//: Public eigenvectors.  After construction, this contains the matrix of
// eigenvectors.
  vnl_matrix<double> V;

//: Public eigenvalues.  After construction, this contains the diagonal
// matrix of eigenvalues, stored as a vector.
  vnl_diag_matrix<double> D;
};

#endif // vnl_generalized_eigensystem_h_
