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
#ifndef vnl_matrix_inverse_h_
#define vnl_matrix_inverse_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_matrix_inverse - vnl_matrix_inverse via vnl_svd<double>
// .LIBRARY     vnl
// .HEADER	vnl Package
// .INCLUDE     vnl/algo/vnl_matrix_inverse.h
// .FILE        vnl/algo/vnl_matrix_inverse.cxx
//
// .SECTION Description
//    vnl_matrix_inverse is a wrapper around vnl_svd<double> that allows you to write
//    x = vnl_matrix_inverse(A) * b;
//    This is exactly equivalent to x = vnl_svd<double>(A).solve(b); but is arguably
//    clearer, and also allows for the vnl_matrix_inverse class to be changed
//    to use vnl_qr, say.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 22 Nov 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/algo/vnl_svd.h>

template <class T>
struct vnl_matrix_inverse : public vnl_svd<T> {
  vnl_matrix_inverse(vnl_matrix<T> const & M): vnl_svd<T>(M) { }
  ~vnl_matrix_inverse() {};
  
  operator vnl_matrix<T> () const { return inverse(); }
};

template <class T>
inline
vnl_vector<T> operator*(vnl_matrix_inverse<T> const & i, vnl_vector<T> const & B)
{
  return i.solve(B);
}

template <class T>
inline
vnl_matrix<T> operator*(vnl_matrix_inverse<T> const & i, vnl_matrix<T> const & B)
{
  return i.solve(B);
}

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_matrix_inverse.
