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
#ifndef PairSetCorner_h_
#define PairSetCorner_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : PairSetCorner
//
// .SECTION Description
//    Extract the matches from a PairMatchSetCorner.  Copies
//    out the inliers into two arrays of HomgPoint2D, and remembers the indices.
//
// .NAME        PairSetCorner - Copy matches out of PairMatchSetCorner
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/PairSetCorner.h
// .FILE        PairSetCorner.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Sep 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PairMatchSetCorner.h>

class PairSetCorner {
public:
  // Constructors/Destructors--------------------------------------------------
  
  PairSetCorner(const PairMatchSetCorner& matches);

  vcl_vector<HomgPoint2D> points1;
  vcl_vector<HomgPoint2D> points2;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS PairSetCorner.

