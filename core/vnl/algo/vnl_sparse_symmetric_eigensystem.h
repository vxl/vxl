// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1998 TargetJr Consortium
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
#ifndef vnl_sparse_symmetric_eigensystem_h_
#define vnl_sparse_symmetric_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vnl_sparse_symmetric_eigensystem - Solve $A x = \lambda x$ using
// Lanczos algorithm.
// .LIBRARY     vnl/algo
// .HEADER	Numerics Package
// .INCLUDE     vnl/algo/vnl_sparse_symmetric_eigensystem.h
// .FILE        vnl/algo/vnl_sparse_symmetric_eigensystem.cxx
//
// .SECTION Description
// @{
//    Solve the eigenproblem $A x = \lambda x$, with $A$ symmetric and
//    sparse.  The block Lanczos algorithm is used to allow the
//    recovery of a number of eigenvale/eigenvector pairs from either
//    end of the spectrum, to a required accuracy.
//
//    Uses the dnlaso routine from the LASO package of netlib. 
// @}
//
// .SECTION Author
//     Rupert W. Curwen, GE CR&D, 20 Oct 98
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_sparse_matrix.h>
#include <vcl/vcl_vector.h>

//: Solve (A x = lambda x) using Lanczos algorithm.

class vnl_sparse_symmetric_eigensystem {
public:
  // Constructors/Destructors--------------------------------------------------
  
  // -- @{ Solve real symmetric eigensystem $A x = \lambda x$ @}
  vnl_sparse_symmetric_eigensystem();
  
public:
  // -- Find n eigenvalue/eigenvectors.  If smallest is true, will
  // calculate the n smallest eigenpairs, else the n largest.
  int CalculateNPairs(vnl_sparse_matrix<double>& M, int n, 
		      bool smallest = true, int nfigures = 10);

public:
  // Operations----------------------------------------------------------------
  // -- Recover specified eigenvector after computation.  The argument
  // must be less than the requested number of eigenvectors.
  vnl_vector<double> get_eigenvector(int i) const;
  double get_eigenvalue(int i) const;

  // Used as a callback in solving.
  int CalculateProduct(int n, int m, const double* p, double* q);
  int SaveVectors(int n, int m, const double* q, int base);
  int RestoreVectors(int n, int m, double* q, int base);

protected:

  vnl_vector<double>* vectors;
  double* values;
  int nvalues;
  vnl_sparse_matrix<double>* mat;

  vcl_vector<double*> temp_store;

};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_sparse_symmetric_eigensystem.
