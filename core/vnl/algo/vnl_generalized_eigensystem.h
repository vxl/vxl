// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_generalized_eigensystem_h_
#define vnl_generalized_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_generalized_eigensystem
//
// .SECTION Description
//    Solve the generalized eigenproblem of @{ $A x = \lambda B x$, with $A$
//    symmetric and $B$ positive definite.  See Golub and van Loan,
//    Section 8.7.
//
//    Uses the EISPACK routine RSG.
// @}
//
// .NAME        vnl_generalized_eigensystem - Symmetric-definite generalized eigensystem
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_generalized_eigensystem.h
// .FILE        vnl/algo/vnl_generalized_eigensystem.cxx

// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 29 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_diag_matrix.h>

//: Symmetric-definite generalized eigensystem

class vnl_generalized_eigensystem {
public:
  // Public data members because they're unique.
  int n;

// -- @{ Solve real generalized eigensystem $A x = \lambda B x$ for
// $\lambda$ and $x$, where $A$ symmetric, $B$ positive definite.
// Initializes storage for the matrix $V = [ x_0 x_1 .. x_n ]$ and
// the vnl_diag_matrix $D = [ \lambda_0 \lambda_1 ... \lambda_n ]$.
// The eigenvalues are sorted into increasing order (of value, not
// absolute value).
// \par
// Uses vnl_cholesky decomposition $C^\top C = B$, to convert to 
// $C^{-\top} A C^{-1} x = \lambda x$ and then uses the
// Symmetric eigensystem code.   It will print a verbose warning
// if $B$ is not positive definite.
// @}
  vnl_generalized_eigensystem(const vnl_matrix<double>& A, const vnl_matrix<double>& B);

// -- Public eigenvcl_vcl_vcl_vectors.  After construction, this contains the matrix of
// eigenvcl_vcl_vcl_vectors.
  vnl_matrix<double> V;

// -- Public eigenvalues.  After construction, this contains the diagonal
// matrix of eigenvalues, stored as a vcl_vcl_vcl_vector.
  vnl_diag_matrix<double> D;

private:
  void compute_eispack(const vnl_matrix<double>& A, const vnl_matrix<double>& B);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_generalized_eigensystem.

