#ifndef HomgOperator3D_h_
#define HomgOperator3D_h_
//:
// \file
// \brief operators for the 3D homogeneous primitive classes
//
// Useful operators for the 3D homogeneous primitive classes.
//
// \verbatim
// Modifications
//   221198 - Peter Vanroose - Added CrossRatio()
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <mvl/HomgLine3D.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgPlane3D.h>

class HomgOperator3D
{
 public:
  static double angle_between_oriented_lines (const HomgLine3D& line1, const HomgLine3D& line2);
  static double distance_squared (const HomgPoint3D& point1, const HomgPoint3D& point2);
  static HomgPoint3D intersect_line_and_plane (const HomgLine3D&, const HomgPlane3D&);
  static HomgPoint3D lines_to_point (const HomgLine3D& line1, const HomgLine3D& line2);
  static HomgPoint3D lines_to_point (const std::vector<HomgLine3D>& line_list);
  static double perp_dist_squared (const HomgPoint3D& point, const HomgLine3D& line);
  static HomgLine3D perp_line_through_point (const HomgLine3D& line, const HomgPoint3D& point);
  static HomgPoint3D perp_projection (const HomgLine3D& line, const HomgPoint3D& point);
  static HomgLine3D planes_to_line (const HomgPlane3D& plane1, const HomgPlane3D& plane2);
  static HomgLine3D planes_to_line (const std::vector<HomgPlane3D>& plane_list);
  static HomgLine3D points_to_line (const HomgPoint3D& point1, const HomgPoint3D& point2);
  static HomgLine3D points_to_line (const std::vector<HomgPoint3D>& point_list);

  static HomgPlane3D points_to_plane (const HomgPoint3D& , const HomgPoint3D& , const HomgPoint3D& );
  static HomgPlane3D points_to_plane (const std::vector<HomgPoint3D>& point_list);
  static HomgPoint3D intersection_point (const HomgPlane3D& , const HomgPlane3D& , const HomgPlane3D& );
  static HomgPoint3D intersection_point (const std::vector<HomgPlane3D>&);

  static void sort_points(HomgPoint3D* p, int n);

  // cross ratio of four collinear points, or four planes through a common line
  static double CrossRatio(const Homg3D& p1, const Homg3D& p2,
                           const Homg3D& p3, const Homg3D& p4);
};

#endif // HomgOperator3D_h_
