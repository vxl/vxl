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
#ifndef vnl_fastops_h_
#define vnl_fastops_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_fastops
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_fastops.h
// .FILE        vnl/vnl_fastops.cxx
//
// .SECTION Description
//    vnl_fastops is a collection of C-style matrix functions for the most
//    time-critical applications.  In general, however one should consider
//    using the vnl_transpose envelope-letter class to achieve the same results
//    with about a 10% speed penalty.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Dec 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matops.h>

class vnl_fastops {
public:
  static void AtA(const vnl_matrix<double>& A, vnl_matrix<double>* out);

  static void AB(const vnl_matrix<double>& A, const vnl_matrix<double>& B, vnl_matrix<double>* out);
  static void AtB(const vnl_matrix<double>& A, const vnl_matrix<double>& B, vnl_matrix<double>* out);
  static void AtB(const vnl_matrix<double>& A, const vnl_vector<double>& B, vnl_vector<double>* out);
  static void ABt(const vnl_matrix<double>& A, const vnl_matrix<double>& B, vnl_matrix<double>* out);

  static void inc_X_by_AtB(vnl_matrix<double>& X, const vnl_matrix<double>& A, const vnl_matrix<double>& B);
  static void inc_X_by_AtB(vnl_vector<double>& X, const vnl_matrix<double>& A, const vnl_vector<double>& B);
  static void inc_X_by_AtA(vnl_matrix<double>& X, const vnl_matrix<double>& A);

  static void dec_X_by_AtB(vnl_matrix<double>& X, const vnl_matrix<double>& A, const vnl_matrix<double>& B);
  static void dec_X_by_AtA(vnl_matrix<double>& X, const vnl_matrix<double>& A);

  static void dec_X_by_ABt(vnl_matrix<double>& X, const vnl_matrix<double>& A, const vnl_matrix<double>& B);

  // BLAS-like operations
  static double dot(const double* a, const double* b, int n);
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_fastops.

