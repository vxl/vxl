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
//--------------------------------------------------------------
//
// .NAME HomgOperator3D - operators for the 3D homogeneous primitive classes
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/HomgOperator3D.h
// .FILE HomgOperator3D.cxx
//
// .SECTION Description:
// Useful operators for the 3D homogeneous primitive classes.
//
// .SECTION Modifications
//   221198 - Peter Vanroose - Added CrossRatio()

#ifndef _HomgOperator3D_h
#define _HomgOperator3D_h

#include <vcl/vcl_vector.h>

#include <mvl/HomgLine3D.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgPlane3D.h>

class HomgOperator3D {

public:
  
  static double angle_between_oriented_lines (const HomgLine3D& line1, const HomgLine3D& line2);
  static double distance_squared (const HomgPoint3D& point1, const HomgPoint3D& point2);
  static HomgPoint3D intersect_line_and_plane (const HomgLine3D&, const HomgPlane3D&);
  static HomgPoint3D lines_to_point (const HomgLine3D& line1, const HomgLine3D& line2);
  static HomgPoint3D lines_to_point (const vcl_vector<HomgLine3D>& line_list);
  static double perp_distance_squared (const HomgLine3D& line, const HomgPoint3D& point);
  static HomgLine3D perp_line_through_point (const HomgLine3D& line, const HomgPoint3D& point);
  static HomgPoint3D perp_projection (const HomgLine3D& line, const HomgPoint3D& point);
  static HomgLine3D planes_to_line (const HomgPlane3D& plane1, const HomgPlane3D& plane2);
  static HomgLine3D planes_to_line (const vcl_vector<HomgPlane3D>& plane_list);
  static HomgLine3D points_to_line (const HomgPoint3D& point1, const HomgPoint3D& point2);
  static HomgLine3D points_to_line (const vcl_vector<HomgPoint3D>& point_list);

  static HomgPlane3D points_to_plane (const HomgPoint3D& , const HomgPoint3D& , const HomgPoint3D& );
  static HomgPlane3D points_to_plane (const vcl_vector<HomgPoint3D>& point_list);
  static HomgPoint3D intersection_point (const HomgPlane3D& , const HomgPlane3D& , const HomgPlane3D& );
  static HomgPoint3D intersection_point (const vcl_vector<HomgPlane3D>&);

  static void sort_points(HomgPoint3D* p, int n);
  
  // cross ratio of four colinear points, or four planes through a common line
  static double CrossRatio(const Homg3D& p1, const Homg3D& p2,
                           const Homg3D& p3, const Homg3D& p4);
};

#endif // _HomgOperator3D_h
