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
#ifndef HomgMap2D_h_
#define HomgMap2D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .SECTION Description
//    HomgMap2D represents a mapping between two 2D spaces.
//    It is used to represent a variety of such mappings, including
//    camera calibration and numerical conditioning.
//
// .NAME        HomgMap2D - General 2D projective map.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgMap2D.h
// .FILE        HomgMap2D.h
// .FILE        HomgMap2D.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 06 Nov 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

class HomgMap2D {
public:
  virtual ~HomgMap2D() = 0;
// -- Transform a point from space 1 to space 2.
  virtual HomgPoint2D transform(const HomgPoint2D& p) = 0;
// -- Transform a point from space 2 to space 1.
  virtual HomgPoint2D inverse_transform(const HomgPoint2D& p) = 0;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgMap2D.
