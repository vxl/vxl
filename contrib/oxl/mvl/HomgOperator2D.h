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
//-*- c++ -*-------------------------------------------------------------
#ifndef _HomgOperator2D_h
#define _HomgOperator2D_h
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME HomgOperator2D - operators for the 2D homogeneous primitive classes
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgOperator2D.h
// .FILE HomgOperator2D.h
// .FILE HomgOperator2D.C
//
// .SECTION Description:
// Useful operators for the 2D homogeneous primitive classes.
//
// .SECTION Modifications
//   221198 - Peter Vanroose - Added CrossRatio() and Conjugate()

//

#include <vcl/vcl_vector.h>

class Homg2D;
class HomgLine2D;
class HomgPoint2D;
class HomgLineSeg2D;

class HomgOperator2D {

public:

  static double dot(const Homg2D& a, const Homg2D& b);
  static void cross(const Homg2D& a, const Homg2D& b, Homg2D* a_cross_b);
  static void unitize(Homg2D* a);
  
  static double angle_between_oriented_lines (const HomgLine2D& line1, const HomgLine2D& line2);
  static double abs_angle (const HomgLine2D& line1, const HomgLine2D& line2);
  
  static double distance_squared (const HomgPoint2D& point1, const HomgPoint2D& point2);
  static double distance_squared (const HomgLineSeg2D& lineseg, const HomgLine2D& line);
  static double distance_squared (const HomgLineSeg2D& lineseg, const HomgPoint2D& line);
  static double distance (const HomgLineSeg2D& lineseg1, const HomgLineSeg2D& lineseg2, double OVERLAP_THRESH);

  static double perp_distance_squared (const HomgLine2D& line, const HomgPoint2D& point);
  static double perp_dist_squared (const HomgPoint2D& point, const HomgLine2D& line);
  static bool is_within_distance(const HomgPoint2D& p1, const HomgPoint2D& p2, double d) {
    return distance_squared(p1, p2) < d*d;
  }

  static double line_angle (const HomgLine2D& line);

  static HomgLine2D join (const HomgPoint2D& point1, const HomgPoint2D& point2);
  static HomgLine2D join_oriented (const HomgPoint2D& point1, const HomgPoint2D& point2);
  static HomgPoint2D intersection (const HomgLine2D& line1, const HomgLine2D& line2);
  static HomgLine2D perp_line_through_point (const HomgLine2D& line, const HomgPoint2D& point);

  static HomgPoint2D perp_projection (const HomgLine2D& line, const HomgPoint2D& point);
  static HomgPoint2D midpoint (const HomgPoint2D& p1, const HomgPoint2D& p2);

  // Clip to lineseg. The infinite line is clipped against the viewport with
  // lower left corner (x0,y0) and upper right corner (x1,y1)
  static HomgLineSeg2D clip_line_to_lineseg (const HomgLine2D& line,
					     double x0, double y0,
					     double x1, double y1);

  // "Intersect" a set of lines
  static HomgPoint2D lines_to_point(const vcl_vector<HomgLine2D>&);

  // cross ratio of four colinear points, or four concurrent lines
  static double CrossRatio(const Homg2D& p1, const Homg2D& p2,
                           const Homg2D& p3, const Homg2D& p4);
  static Homg2D Conjugate(const Homg2D& a, const Homg2D& b, const Homg2D& c,
                          double cr);
};

#endif
