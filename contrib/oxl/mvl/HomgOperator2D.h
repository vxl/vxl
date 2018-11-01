// This is oxl/mvl/HomgOperator2D.h
#ifndef HomgOperator2D_h_
#define HomgOperator2D_h_
//:
// \file
// \brief useful operators for the 2D homogeneous primitive classes
//
// \verbatim
// Modifications
//   221198 - Peter Vanroose - Added CrossRatio() and Conjugate()
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class Homg2D;
class HomgLine2D;
class HomgPoint2D;
class HomgLineSeg2D;

class HomgOperator2D
{
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
  static HomgPoint2D lines_to_point(const std::vector<HomgLine2D>&);

  // cross ratio of four collinear points, or four concurrent lines
  static double CrossRatio(const Homg2D& p1, const Homg2D& p2,
                           const Homg2D& p3, const Homg2D& p4);
  static Homg2D Conjugate(const Homg2D& a, const Homg2D& b, const Homg2D& c,
                          double cr);
};

#endif // HomgOperator2D_h_
