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
#ifndef vnl_cholesky_h_
#define vnl_cholesky_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_cholesky - Decomposition of symmetric matrix
// .FILE        vnl/algo/vnl_cholesky.cxx
//
// .SECTION Description
//    A class to hold the Cholesky decomposition of a symmetric matrix and
//    use that to solve linear systems, compute determinants and inverses.
//    The cholesky decomposition decomposes symmetric A = L*L.transpose()
//    where L is lower triangular 
//
// .SECTION See Also
//     vnl_symmetric_eigensystem
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Dec 96
//
// .SECTION Modifications
//     Peter Vanroose, Leuven, Apr 1998: added L() (return decomposition matrix)
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>

class vnl_cholesky {
public:
  // -- Modes of computation.  See constructor for details.
  enum Operation {
    quiet,
    verbose,
    estimate_condition
  };
  
// -- Make cholesky decomposition of M optionally computing
// the reciprocal condition number.
  vnl_cholesky(vnl_matrix<double> const& M, Operation mode = verbose);
 ~vnl_cholesky() {}

  // -- Solve LS problem M x = b
  vnl_vector<double> solve(vnl_vector<double> const& b) const;

  // -- Solve LS problem M x = b
  void solve(vnl_vector<double> const& b, vnl_vector<double>* x) const;
  
  // -- Compute determinant
  double determinant() const;
  
  // -- Compute inverse.  Not efficient.
  vnl_matrix<double> inverse() const;

  // -- Return Upper-triangular factor.
  vnl_matrix<double> upper_triangle() const;

  // -- return the decomposition matrix
  vnl_matrix<double> const& L() { return A_; }

// -- A Success/failure flag
  int rank_deficiency() const { return num_dims_rank_def_; }
  
// -- Return reciprocal condition number.  Not calculated unless Operaton mode
// at construction was estimate_condition.
  double rcond() const { return rcond_; }

// -- Return computed nullvector. Not calculated unless Operaton mode
// at construction was estimate_condition.
  vnl_vector<double>      & nullvector()       { return nullvector_; }
  vnl_vector<double> const& nullvector() const { return nullvector_; }

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  vnl_matrix<double> A_;
  double rcond_;
  int num_dims_rank_def_;
  vnl_vector<double> nullvector_;
  
private:
  // Helpers-------------------------------------------------------------------
  
  // Thatcher's children, privatised...
  vnl_cholesky(vnl_cholesky const & that);
  vnl_cholesky& operator=(vnl_cholesky const & that);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_cholesky.

