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
#ifndef vnl_eigensystem_h_
#define vnl_eigensystem_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_eigensystem - Unsymmetric real eigensystem.
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_eigensystem.h
// .FILE        vnl/algo/vnl_eigensystem.cxx
// .EXAMPLE     vnl/examples/vnl_eigensystem.cxx
//
// .SECTION Description
//    vnl_eigensystem is a full-bore real eigensystem.  If your matrix is symmetric,
//    it is *much* better to use vnl_symmetric_eigensystem.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Jan 97
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_complex.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_diag_matrix.h>

class vnl_real_eigensystem {
public:
  vnl_real_eigensystem(const vnl_matrix<double>& M);
  
public:
  vnl_matrix<double> Vreal;
  
  // -- Output matrix of eigenvectors, which will in general be complex.
  vnl_matrix<vnl_double_complex> V;
  
  // -- Output diagonal matrix of eigenvalues.
  vnl_diag_matrix<vnl_double_complex> D;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_eigensystem.

