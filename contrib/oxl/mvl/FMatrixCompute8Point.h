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
#ifndef FMatrixCompute8Point_h_
#define FMatrixCompute8Point_h_
#ifdef __GNUC__
#pragma interface
#endif
//-----------------------------------------------------------------------------
//
// Class : FMatrixCompute8Point
//
// .SECTION Description
//    FMatrixCompute8Point is a subclass of FMatrixCompute, and implements
//    the ``8-point'' or linear least squares algorithm for estimation of
//    the fundamental matrix.
//
// .NAME        FMatrixCompute8Point - FMatrix by linear least squares 
// .LIBRARY     MViewCompute
// .HEADER	MultiView Package
// .INCLUDE     mvl/FMatrixCompute8Point.h
// .FILE        FMatrixCompute8Point.h
// .FILE        FMatrixCompute8Point.C
// .EXAMPLE     ../Examples/exampleFMatrixCompute.C

//     Andrew W. Fitzgibbon, Oxford IERG, 23 Jul 96
//

#include <mvl/FMatrix.h>
#include <mvl/FMatrixCompute.h>

class FMatrixCompute8Point : public FMatrixCompute {
public:
  // Implement FMatrixCompute interface
  bool compute();
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS FMatrixCompute8Point.
