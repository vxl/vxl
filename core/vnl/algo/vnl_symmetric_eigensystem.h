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
#ifndef vnl_symmetric_eigensystem_h_
#define vnl_symmetric_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_symmetric_eigensystem - @{\boldmath Solve $A x = \lambda x$ using vnl_qr. @}
// .LIBRARY     vnl/algo
// .HEADER	Numerics Package
// .INCLUDE     vnl/algo/vnl_symmetric_eigensystem.h
// .FILE        vnl/algo/vnl_symmetric_eigensystem.cxx
//
// .SECTION Description
// @{
//    Solve the eigenproblem $A x = \lambda x$, with $A$ symmetric.
//    The resulting eigenvectors and values are sorted in increasing order
//    so V.column(0) is the eigenvector corresponding to the smallest
//    eigenvalue.
//
//    Uses the EISPACK routine RS, which in turn calls TRED2 to reduce A
//    to tridiagonal form, followed by TQL2, to find the eigensystem.
//    This is summarized in Golub and van Loan, \S8.2.  The following are
//    the original subroutine headers:
// 
// \begin{quote}\small
//    TRED2 is a translation of the Algol procedure tred2,
//     Num. Math. 11, 181-195(1968) by Martin, Reinsch, and Wilkinson.
//     Handbook for Auto. Comp., Vol.ii-Linear Algebra, 212-226(1971).
//
//     This subroutine reduces a real symmetric matrix to a
//     symmetric tridiagonal matrix using and accumulating
//     orthogonal similarity transformations.
//
//    TQL2 is a translation of the Algol procedure tql2,
//     Num. Math. 11, 293-306(1968) by Bowdler, Martin, Reinsch, and Wilkinson.
//     Handbook for Auto. Comp., Vol.ii-Linear Algebra, 227-240(1971).
//
//     This subroutine finds the eigenvalues and eigenvectors
//     of a symmetric tridiagonal matrix by the QL method.
//     the eigenvectors of a full symmetric matrix can also
//     be found if  tred2  has been used to reduce this
//     full matrix to tridiagonal form.
// \end{quote}
// @}
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 29 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>

class vnl_symmetric_eigensystem {
public:
  //: @{ Solve real symmetric eigensystem $A x = \lambda x$ @}
  vnl_symmetric_eigensystem(const vnl_matrix<double>& M);
  
protected:
  // need this here to get inits in correct order, but still keep gentex
  // in the right order.
  int n_;

public:
  //: Public eigenvectors.  After construction, the columns of V are the
  // eigenvectors, sorted by increasing eigenvalue, from most negative to
  // most positive.
  vnl_matrix<double> V;

  //: Public eigenvalues.  After construction,  D contains the
  // eigenvalues, sorted as described above.  Note that D is a vnl_diag_matrix,
  // and is therefore stored as a vcl_vector while behaving as a matrix.
  vnl_diag_matrix<double> D;

  //: Recover specified eigenvector after computation.
  vnl_vector<double> get_eigenvector(int i) const;

  //: Get least-squares nullvector. convenience method.
  vnl_vector<double> get_nullvector() const { return get_eigenvector(0); }
  
  
  //: @{ Return the matrix $V  D  V^\top$.  This can be useful if you've
  // modified $D$.  So an inverse is obtained using
  // \begin{alltt}
  //   {\bf{}vnl_symmetric_eigensystem} eig(A);
  //   eig.D.{\bf invert\_in\_place}();
  //   vnl_matrix<double> Ainverse = eig.{\bf recompose}();
  // \end{alltt}
  // @}
  vnl_matrix<double> recompose() const { return V * D * V.transpose(); }

  vnl_matrix<double> pinverse() const; // pseudoinverse

  //: Solve LS problem M x = b
  vnl_vector<double> solve(const vnl_vector<double>& b);
  
  //: Solve LS problem M x = b
  void solve(const vnl_vector<double>& b, vnl_vector<double>* x);
  
  static bool compute(const vnl_matrix<double>& in, vnl_matrix<double>& V, vnl_vector<double>& D);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_symmetric_eigensystem.
