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
#ifndef vnl_int_matrix_h_
#define vnl_int_matrix_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME        vnl_int_matrix - Matrix of ints.
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_int_matrix.h
// .FILE        vnl/vnl_int_matrix.cxx
//
// .SECTION Description
//    vnl_int_matrix specializes vnl_matrix for integers, adding a vnl_matrix<double> ctor.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 27 Dec 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>

class vnl_int_matrix : public vnl_matrix<int> {
  typedef vnl_matrix<int> Base;
public:
  
  vnl_int_matrix() {}
  vnl_int_matrix(char const* filename);
  vnl_int_matrix(istream& s);
  vnl_int_matrix(unsigned r, unsigned c): Base(r, c) {}
  vnl_int_matrix(unsigned r, unsigned c, int fillvalue): Base(r, c, fillvalue) {}
  vnl_int_matrix(const vnl_matrix<double>& d);
  vnl_int_matrix(const vnl_matrix<int>& d):Base(d) {}
  vnl_int_matrix& operator = (const vnl_matrix<int>& d) { return (vnl_int_matrix&)Base::operator = (d); }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_int_matrix.

