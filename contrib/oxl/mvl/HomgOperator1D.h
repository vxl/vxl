// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1998 TargetJr Consortium
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
#ifndef HomgOperator1D_h_
#define HomgOperator1D_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        HomgOperator1D - 1D homogeneous functions
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/HomgOperator1D.h
// .FILE        HomgOperator1D.h
// .FILE        HomgOperator1D.C
//
// .SECTION Description:
// HomgOperator1D implements one-dimensional homogeneous functions.
//
// .SECTION Author:
//     Peter Vanroose, ESAT/PSI, Nov. 1998.
//
//-----------------------------------------------------------------------------
#include <mvl/HomgPoint1D.h>

class HomgOperator1D {
public:
  static double CrossRatio(const Homg1D& a, const Homg1D& b,
                           const Homg1D& c, const Homg1D& d);
  static double Conjugate(double x1, double x2, double x3, double cr = -1);
  static Homg1D Conjugate(const Homg1D& a, const Homg1D& b, const Homg1D& c,
                          double cr = -1);

  static double dot(const Homg1D& a, const Homg1D& b);
  static double cross(const Homg1D& a, const Homg1D& b);
  static void unitize(Homg1D* a);
  
  static double distance (const HomgPoint1D& point1, const HomgPoint1D& point2);
  static double distance_squared (const HomgPoint1D& point1, const HomgPoint1D& point2);

  static bool is_within_distance(const HomgPoint1D& p1, const HomgPoint1D& p2, double d) {
    return distance(p1, p2) < d;
  }
  static HomgPoint1D midpoint (const HomgPoint1D& p1, const HomgPoint1D& p2);
};

#endif
