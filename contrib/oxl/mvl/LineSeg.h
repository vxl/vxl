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
#ifndef LineSeg_h_
#define LineSeg_h_
//-----------------------------------------------------------------------------
//
// Class : LineSeg
//
// .SECTION Description
//    LineSeg is a class that awf hasn't documented properly. FIXME
//
// .NAME        LineSeg - Undocumented class FIXME
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/LineSeg.h
// .FILE        LineSeg.h
// .FILE        LineSeg.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 01 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------
#include <vcl/vcl_iosfwd.h>


class LineSeg {

public:

  // Data Members--------------------------------------------------------------

  float _x0;
  float _y0;
  float _x1;
  float _y1;
  float _theta;
  float _grad_mean;

  // Constructors/Destructors--------------------------------------------------

public:
  
  LineSeg() {}
  LineSeg(float x0, float y0, float x1, float y1, float theta, float grad_mean);
  ~LineSeg() {}

  // Data Access---------------------------------------------------------------

public:

  // Data Control--------------------------------------------------------------
  
};

ostream& operator<<(ostream&, const LineSeg& l);
istream& operator>>(istream&, LineSeg& l);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS LineSeg.
