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
#ifndef vnl_double_2x3_h_
#define vnl_double_2x3_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_double_2x3
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_double_2x3.h
// .FILE        vnl/vnl_double_2x3.cxx
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 23 Dec 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3.h>

class vnl_double_2x3 : public vnl_matrix_fixed<double, 2, 3> {
  typedef vnl_matrix_fixed<double, 2, 3> Base;
public:

  vnl_double_2x3() {}
  vnl_double_2x3(const vnl_double_3& row1, const vnl_double_3& row2) {
    vnl_matrix<double>& M = *this;
    M(0,0) = row1[0];    M(0,1) = row1[1];    M(0,2) = row1[2];
    M(1,0) = row2[0];    M(1,1) = row2[1];    M(1,2) = row2[2];
  }

  vnl_double_2x3(double r00, double r01, double r02,
	    double r10, double r11, double r12) {
    vnl_matrix<double>& M = *this;
    M(0,0) = r00;    M(0,1) = r01;    M(0,2) = r02;
    M(1,0) = r10;    M(1,1) = r11;    M(1,2) = r12;
  }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_double_2x3.

