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
#ifndef linearoperators3_h_
#define linearoperators3_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : LinearOperators3
//
// .SECTION Description
//    Specialized linear operators for 3D vectors and matrices.
//    Include this file if you're inlining or compiling linear algebra
//    code for speed.
//
// .NAME        LinearOperators3 - 3D linear algebra operations
// .LIBRARY     vnl
// .HEADER	Numerics package
// .INCLUDE     vnl/LinearOperators3.h
// .FILE        vnl/LinearOperators3.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

// --
inline
vnl_double_3 operator* (const vnl_double_3x3& A, const vnl_double_3& x)
{
  const double* a = A.data_block();
  double r0 = a[0] * x[0] + a[1] * x[1] + a[2] * x[2];
  double r1 = a[3] * x[0] + a[4] * x[1] + a[5] * x[2];
  double r2 = a[6] * x[0] + a[7] * x[1] + a[8] * x[2];
  return vnl_double_3(r0, r1, r2);
}

// -- 
inline
vnl_double_3 operator+ (const vnl_double_3& a, const vnl_double_3& b)
{
  double r0 = a[0] + b[0];
  double r1 = a[1] + b[1];
  double r2 = a[2] + b[2];
  return vnl_double_3(r0, r1, r2);
}

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS LinearOperators3.
