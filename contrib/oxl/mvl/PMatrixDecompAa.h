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
#ifndef PMatrixDecompAa_h_
#define PMatrixDecompAa_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : PMatrixDecompAa
//
// .SECTION Description
//    Decompose PMatrix into [A a] where A is 3x3 and a is 3x1.
//
// .NAME        PMatrixDecompAa - Decompose PMatrix into [A a]
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/PMatrixDecompAa.h
// .FILE        PMatrixDecompAa.h
// .FILE        PMatrixDecompAa.C
// .EXAMPLE     ../Examples/examplePMatrixDecompAa.cc
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 14 Feb 97

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <mvl/PMatrix.h>

class PMatrixDecompAa {
public:

// -- Public data members for A and a.
  vnl_double_3x3 A;
  vnl_double_3   a;
  //{genman, do not do anything to this line -- awf}

// -- Default constructor
  PMatrixDecompAa() {}

// -- Construct from PMatrix.
  PMatrixDecompAa(const PMatrix& P) { set(P); }

// --  Set [A a] from PMatrix.
  void set(const PMatrix& P) { P.get(&A, &a); }

// --  Set PMatrix from [A a].
  void get(PMatrix* P) const { P->set(A, a); }
  
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS PMatrixDecompAa.
