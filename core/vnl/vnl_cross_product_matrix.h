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
#ifndef vnl_cross_product_matrix_h_
#define vnl_cross_product_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_cross_product_matrix
//
// .SECTION Description
//    @{ vnl_cross_product_matrix forms the $3 \times 3$ skew symmetric cross product
//    matrix from a vector.
//    
//    \noindent {\tt vnl_cross_product_matrix(e)} is the matrix $[e]_\times$:
//   \[
//   \left( \begin{array}{ccc}
//          0  & -e_3  &  e_2 \\ 
//        e_3  &  0    & -e_1 \\ 
//       -e_2  &  e_1  &  0
//   \end{array} \right)
//   \]@}
//      
//
// .NAME        vnl_cross_product_matrix - 3x3 cross-product matrix of vector
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_cross_product_matrix.h
// .FILE        vnl/vnl_cross_product_matrix.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 19 Sep 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>

class vnl_cross_product_matrix : public vnl_double_3x3 {
public:
  typedef vnl_double_3x3 base;
  
  vnl_cross_product_matrix(const vnl_vector<double>& v) { set(v.data_block()); }
  vnl_cross_product_matrix(const double* v) { set(v); }
  vnl_cross_product_matrix(const vnl_cross_product_matrix& that): base(that) {}
 ~vnl_cross_product_matrix() {}

  vnl_cross_product_matrix& operator=(const vnl_cross_product_matrix& that) {
    base::operator= (that);
    return *this;
  }

  void set(const double* v); // override method in vnl_vector
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_cross_product_matrix.

