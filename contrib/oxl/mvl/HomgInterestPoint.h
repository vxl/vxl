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
#ifndef HomgInterestPoint_h_
#define HomgInterestPoint_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : HomgInterestPoint
//
// .SECTION Description
//    HomgInterestPoint is a representation of an interest point
//    resulting from feature detection.  At Oxford this generally
//    means a Harris corner.
//
// .NAME        HomgInterestPoint - Homogeneous interest point/corner.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgInterestPoint.h
// .FILE        HomgInterestPoint.h
// .FILE        HomgInterestPoint.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 17 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_iosfwd.h>

#include <mvl/HomgPoint2D.h>

class HomgMetric;

class HomgInterestPoint {
  // Data Members--------------------------------------------------------------
public:
  HomgPoint2D _homg;
  vnl_double_2 _double2;
  vnl_vector_fixed<int, 2> _int2;
public:
  float       _mean_intensity;

  HomgInterestPoint();
  HomgInterestPoint(double x, double y, float mean_intensity = 0.0F);
  HomgInterestPoint(double x, double y, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgPoint2D& h, const HomgMetric& c, float mean_intensity = 0.0F);
  HomgInterestPoint(const HomgInterestPoint&); // copy constructor
  HomgInterestPoint& operator=(const HomgInterestPoint&); // assignment
  ~HomgInterestPoint();

  friend bool operator == (const HomgInterestPoint&, const HomgInterestPoint&);
};

ostream& operator << (ostream& s, const HomgInterestPoint&);

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS HomgInterestPoint.

