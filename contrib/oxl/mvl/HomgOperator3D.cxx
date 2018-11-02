#include <cmath>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include "HomgOperator3D.h"
//:
//  \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix_fixed.h>

#include <mvl/HomgLine3D.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgPlane3D.h>

// -----------------------------------------------------------------------------

//: Given collinear 3D points in random order, sort them.
void
HomgOperator3D::sort_points(HomgPoint3D* points, int n)
{
  if (n <= 0) return; // early return if possible

  // ho_quadvecstd_sort_quadvecs

  int num_finite = 0;
  HomgPoint3D finite_quadvec;
  for (int p_index = 0; p_index < n; p_index++) {
    HomgPoint3D* p = &points[p_index];
    if (p->w() != 0) {
      ++num_finite;
      finite_quadvec = *p;
    }
  }
  if (num_finite < n) {
    std::cerr << "WARNING HomgOperator3D::sort_points -- "
             << (n - num_finite) << " points at infinity\n";
  }

  if (num_finite == 0) {
    std::cerr << "HomgOperator3D::sort_points: all points at infinity - no action\n";
    return;
  }

  double distance_max = 0;
  HomgLine3D line;
  for (int p_index = 0; p_index < n; p_index++) {
    HomgPoint3D* p = &points[p_index];
    if (p->w() != 0) {
      double distance = HomgOperator3D::distance_squared(finite_quadvec, *p);
      if (distance > distance_max) {
        distance_max = distance;
        // ho_quadvecstd_points2_to_line
        line = HomgLine3D(finite_quadvec, *p);
      }
    }
  }

  /* now create a point which is bound to be beyond the endpoints of the set. */
  vnl_double_3 finite_trivec = finite_quadvec.get_double3();

//vnl_double_3 start = line.get_point_finite().get_double3();
  vnl_double_3 dir = line.dir();

  vnl_double_3 faraway_trivec = finite_trivec + 2.0 * distance_max * dir;
  HomgPoint3D faraway(faraway_trivec[0], faraway_trivec[1], faraway_trivec[2], 1.0);

  typedef std::pair<float, int> pair_float_int;
  std::vector< pair_float_int > sort_table(n);
  std::vector< HomgPoint3D > tempoints(points, points + n);

  for (int p_index = 0; p_index < n; p_index++) {
    HomgPoint3D* p = &points[p_index];
    if (p->w() != 0) {
      sort_table[p_index].first = (float)HomgOperator3D::distance_squared(faraway, *p);
      sort_table[p_index].second = p_index;
    }
  }

  std::sort(sort_table.begin(), sort_table.end());

  for (int sort_index = 0; sort_index < n; sort_index++) {
    pair_float_int* sort = &sort_table[sort_index];
    tempoints[sort_index] = points[sort->second];
  }

  for (int i = 0; i < n; ++i)
    points[i] = tempoints[i];
}

//-----------------------------------------------------------------------------
//
//: Return the angle between the (oriented) lines (in radians)
//
double
HomgOperator3D::angle_between_oriented_lines (const HomgLine3D& l1, const HomgLine3D& l2)
{
  HomgPoint3D const& dir1 = l1.get_point_infinite();
  HomgPoint3D const& dir2 = l2.get_point_infinite();
  double n = dir1.x()*dir1.x()+dir1.y()*dir1.y()+dir1.z()*dir1.z();
  n       *= dir2.x()*dir2.x()+dir2.y()*dir2.y()+dir2.z()*dir2.z();
  // dot product of unit direction vectors:
  n = (dir1.x()*dir2.x()+dir1.y()*dir2.y()+dir1.z()*dir2.z())/std::sqrt(n);
  return std::acos(n);
}


//-----------------------------------------------------------------------------
//
//: Return the squared distance between the points
//
double
HomgOperator3D::distance_squared (const HomgPoint3D& point1, const HomgPoint3D& point2)
{
  return (point1.get_double3() - point2.get_double3()).magnitude();
}

//-----------------------------------------------------------------------------
//
//: Return the intersection point of the line and plane
//
HomgPoint3D
HomgOperator3D::intersect_line_and_plane (const HomgLine3D &line, const HomgPlane3D& plane)
{
  //
  /* use P.(S + lambda D) = 0 to find lambda, and hence a point on the plane. */

  const vnl_double_4& x1 = line.get_point_finite().get_vector();
  const vnl_double_4& x2 = line.get_point_infinite().get_vector();
  const vnl_double_4& p  = plane.get_vector();

  double numerator = -dot_product (x1, p);
  double denominator = dot_product (x2, p);

  // Scale for conditioning
  double scale = 1.0/(numerator + denominator);
  numerator *= scale;
  denominator *= scale;

  return denominator * x1 + numerator * x2;
}

//-----------------------------------------------------------------------------
//
// - Compute the intersection point of the lines, or the mid-point
// of the common perpendicular if the lines are skew
//
HomgPoint3D
HomgOperator3D::lines_to_point (const HomgLine3D& , const HomgLine3D& )
{
  std::cerr << "Warning: HomgOperator3D::lines_to_point() not yet implemented\n";
  return HomgPoint3D();
}


//-----------------------------------------------------------------------------
//
// - Compute the best fit intersection point of the lines
//
HomgPoint3D
HomgOperator3D::lines_to_point (const std::vector<HomgLine3D>& )
{
  std::cerr << "Warning: HomgOperator3D::lines_to_point() not yet implemented\n";
  return HomgPoint3D();
}

//-----------------------------------------------------------------------------
//
// - Return the squared perpendicular distance between the line and point
//
double
HomgOperator3D::perp_dist_squared (const HomgPoint3D& , const HomgLine3D& )
{
  std::cerr << "Warning: HomgOperator3D::perp_dist_squared() not yet implemented\n";
  return 0;
}

//-----------------------------------------------------------------------------
//
// - Return the line which is perpendicular to *line and passes
// through *point
//
HomgLine3D
HomgOperator3D::perp_line_through_point (const HomgLine3D& , const HomgPoint3D& ) { return HomgLine3D(); }


//-----------------------------------------------------------------------------
//
// - Compute the line which is the perpendicular projection of *point
// onto *line
//
HomgPoint3D
HomgOperator3D::perp_projection (const HomgLine3D& , const HomgPoint3D& )
{
  std::cerr << "Warning: HomgOperator3D::perp_projection() not yet implemented\n";
  return HomgPoint3D();
}


//-----------------------------------------------------------------------------
//
//: Return the intersection line of the planes
//
HomgLine3D
HomgOperator3D::planes_to_line (const HomgPlane3D& plane1, const HomgPlane3D& plane2)
{
  vnl_matrix_fixed<double,2,4> M;
  M.set_row(0, plane1.get_vector());
  M.set_row(1, plane2.get_vector());
  vnl_svd<double> svd(M.as_ref());
  vnl_matrix_fixed<double,4,2> N = svd.nullspace(2);
  HomgPoint3D p1(N.get_column(0));
  HomgPoint3D p2(N.get_column(1));
  return HomgLine3D(p1, p2);
}


//-----------------------------------------------------------------------------
//
// - Compute the best-fit intersection line of the planes
//
HomgLine3D
HomgOperator3D::planes_to_line (const std::vector<HomgPlane3D>&)
{
  std::cerr << "Warning: HomgOperator3D::planes_to_line() not yet implemented\n";
  return HomgLine3D();
}


//-----------------------------------------------------------------------------
//
// - Return the line through the points
//
HomgLine3D
HomgOperator3D::points_to_line (const HomgPoint3D&, const HomgPoint3D&)
{
  std::cerr << "Warning: HomgOperator3D::points_to_line() not yet implemented\n";
  return HomgLine3D();
}

//-----------------------------------------------------------------------------
//
// - Compute the best-fit line through the points
//
HomgLine3D
HomgOperator3D::points_to_line (const std::vector<HomgPoint3D>&)
{
  std::cerr << "Warning: HomgOperator3D::points_to_line() not yet implemented\n";
  return HomgLine3D();
}

//-----------------------------------------------------------------------------
//
// - Return the plane through the points
//
HomgPlane3D
HomgOperator3D::points_to_plane (const HomgPoint3D&, const HomgPoint3D&, const HomgPoint3D&)
{
  std::cerr << "Warning: HomgOperator3D::points_to_plane() not yet implemented\n";
  return HomgPlane3D();
}


//-----------------------------------------------------------------------------
//
// - Compute the best-fit plane through the points
//
HomgPlane3D
HomgOperator3D::points_to_plane (const std::vector<HomgPoint3D>&)
{
  std::cerr << "Warning: HomgOperator3D::points_to_plane() not yet implemented\n";
  return HomgPlane3D();
}

//: Compute best-fit intersection of planes in a point.

HomgPoint3D
HomgOperator3D::intersection_point (const HomgPlane3D& plane1, const HomgPlane3D& plane2, const HomgPlane3D& plane3)
{
  vnl_matrix<double> A(3, 4);
  A(0,0) = plane1.x();
  A(0,1) = plane1.y();
  A(0,2) = plane1.z();
  A(0,3) = plane1.w();

  A(1,0) = plane2.x();
  A(1,1) = plane2.y();
  A(1,2) = plane2.z();
  A(1,3) = plane2.w();

  A(2,0) = plane3.x();
  A(2,1) = plane3.y();
  A(2,2) = plane3.z();
  A(2,3) = plane3.w();

  vnl_svd<double> svd(A);
  return HomgPoint3D(svd.nullvector());
}

HomgPoint3D
HomgOperator3D::intersection_point (const std::vector<HomgPlane3D>& planes)
{
  int n = planes.size();
  vnl_matrix<double> A(planes.size(), 4);

  for (int i =0; i < n; ++i) {
    A(i,0) = planes[i].x();
    A(i,1) = planes[i].y();
    A(i,2) = planes[i].z();
    A(i,3) = planes[i].w();
  }

  vnl_svd<double> svd(A);
  return HomgPoint3D(svd.nullvector());
}

//-----------------------------------------------------------------------------
//: Calculates the crossratio of four collinear points p1, p2, p3 and p4.
// This number is projectively invariant, and it is the coordinate of p4
// in the reference frame where p2 is the origin (coordinate 0), p3 is
// the unity (coordinate 1) and p1 is the point at infinity.
// This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
// equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
// and is calculated as
//                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
//                      ------- : --------  =  --------------
//                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
//
// In principle, any single nonhomogeneous coordinate from the four points
// can be used as parameters for CrossRatio (but of course the same for all
// points). The most reliable answer will be obtained when the coordinate with
// the largest spacing is used, i.e., the one with smallest slope.
//
double HomgOperator3D::CrossRatio(const Homg3D& a, const Homg3D& b, const Homg3D& c, const Homg3D& d)
{
  double x1 = a.x(), y1 = a.y(), z1 = a.z(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), z2 = b.z(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), z3 = c.z(), w3 = c.w();
  double x4 = d.x(), y4 = d.y(), z4 = d.z(), w4 = d.w();
  double x = x1 - x2; if (x<0) x = -x; // assuming a != b ;-)
  double y = y1 - y2; if (y<0) y = -y;
  double z = z1 - z2; if (z<0) z = -z;
  double n = (x>y && x>z) ? (x1*w3-x3*w1)*(x2*w4-x4*w2) :
             (y>z)        ? (y1*w3-y3*w1)*(y2*w4-y4*w2) :
                            (z1*w3-z3*w1)*(z2*w4-z4*w2);
  double m = (x>y && x>z) ? (x1*w4-x4*w1)*(x2*w3-x3*w2) :
             (y>z)        ? (y1*w4-y4*w1)*(y2*w3-y3*w2) :
                            (z1*w4-z4*w1)*(z2*w3-z3*w2);
  if (n == 0 && m == 0)
    std::cerr << "CrossRatio not defined: three of the given points coincide\n";
  return n/m;
}
