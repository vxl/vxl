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
#ifndef vnl_transpose_h_
#define vnl_transpose_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_transpose - Efficient matrix transpose.
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_transpose.h
// .FILE        vnl/vnl_transpose.cxx
//
// .SECTION Description
//    vnl_transpose is an efficient way to write C = vnl_transpose(A) * B.
//    The vnl_transpose class holds a reference to the original matrix
//    and when involved in an operation for which it has been specialized,
//    performs the operation without copying.
//
//    If the operation has not been specialized, the vnl_transpose performs
//    a copying conversion to a matrix, printing a message to stdout.
//    At that stage, the user may choose to implement the particular operation
//    or use vnl_transpose::asMatrix() to clear the warning.
//
//    NOTE: This is a reference class, so should be shorted-lived than the
//    matrix to which it refers.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Dec 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matops.h>
#include <vnl/vnl_fastops.h>

class vnl_transpose {
  const vnl_matrix<double>& M_;
public:

// -- Make a vnl_transpose object referring to matrix M
  vnl_transpose(const vnl_matrix<double>& M): M_(M) {}
  
// -- Noisily convert a vnl_transpose to a matrix
  operator vnl_matrix<double> () const {
    cerr << "vnl_transpose being converted to matrix -- help! I don't wanna go!\n";
    return M_.transpose();
  }

// -- Quietly convert a vnl_transpose to a matrix
  vnl_matrix<double> asMatrix () const {
    return M_.transpose();
  }

// -- Return M' * O
  vnl_matrix<double> operator* (const vnl_matrix<double>& O) {
    vnl_matrix<double> ret(M_.columns(), O.columns());
    vnl_fastops::AtB(M_, O, &ret);
    return ret;
  }

// -- Return M' * O
  vnl_vector<double> operator* (const vnl_vector<double>& O) {
    vnl_vector<double> ret(M_.columns());
    vnl_fastops::AtB(M_, O, &ret);
    return ret;
  }

// -- Return A * B'
  friend vnl_matrix<double> operator* (const vnl_matrix<double>& A, const vnl_transpose& B) {
    vnl_matrix<double> ret(A.rows(), B.M_.rows());
    vnl_fastops::ABt(A, B.M_, &ret);
    return ret;
  }

};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_transpose.
