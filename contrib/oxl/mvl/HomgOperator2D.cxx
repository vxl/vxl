// This is oxl/mvl/HomgOperator2D.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "HomgOperator2D.h"

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_scatter_3x3.h>
#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLineSeg2D.h>

// == BASICS ==

//-----------------------------------------------------------------------------
//: Cross product of two Homg2Ds
void HomgOperator2D::cross(const Homg2D& a, const Homg2D& b, Homg2D* a_cross_b)
{
  double x1 = a.x();
  double y1 = a.y();
  double w1 = a.w();

  double x2 = b.x();
  double y2 = b.y();
  double w2 = b.w();

  a_cross_b->set (y1 * w2 - w1 * y2,
                  w1 * x2 - x1 * w2,
                  x1 * y2 - y1 * x2);
}

//-----------------------------------------------------------------------------
//: Dot product of two Homg2Ds
double HomgOperator2D::dot(const Homg2D& a, const Homg2D& b)
{
  double x1 = a.x();
  double y1 = a.y();
  double w1 = a.w();

  double x2 = b.x();
  double y2 = b.y();
  double w2 = b.w();

  return x1*x2 + y1*y2 + w1*w2;
}

//-----------------------------------------------------------------------------
//: Normalize Homg2D to unit magnitude
void HomgOperator2D::unitize(Homg2D* a)
{
  double norm = a->x()*a->x() + a->y()*a->y() + a->w()*a->w();

  if (norm == 0.0) {
    vcl_cerr << "HomgOperator2D::unitize() -- Zero length vector\n";
    return;
  }

  norm = 1.0/vcl_sqrt(norm);
  a->set(a->x()*norm, a->y()*norm, a->w()*norm);
}

// == DISTANCE MEASUREMENTS IN IMAGE COORDINATES ==

//: Get the square of the 2D distance between the two points.
double HomgOperator2D::distance_squared(const HomgPoint2D& point1,
                                        const HomgPoint2D& point2)
{
  double x1 = point1.x();
  double y1 = point1.y();
  double z1 = point1.w();

  double x2 = point2.x();
  double y2 = point2.y();
  double z2 = point2.w();

  if (z1 == 0 || z2 == 0) {
    vcl_cerr << "HomgOperator2D::distance_squared() -- point at infinity";
    return Homg::infinity;
  }

  double scale1 = 1.0/z1;
  double scale2 = 1.0/z2;

  return vnl_math_sqr (x1 * scale1 - x2 * scale2) +
         vnl_math_sqr (y1 * scale1 - y2 * scale2);
}

//-----------------------------------------------------------------------------
//
//: Get the square of the perpendicular distance to a line.
// This is just the homogeneous form of the familiar
// $\frac{a x + b y + c}{\sqrt{a^2+b^2}}$:
// \f\[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \f\]
// If either the point or the line are at infinity an error message is
// printed and Homg::infinity is returned.
double HomgOperator2D::perp_dist_squared (const HomgPoint2D& point, const HomgLine2D& line)
{
  if (line.ideal(0.0) || point.ideal(0.0)) {
    vcl_cerr << "HomgOperator2D::perp_dist_squared() -- line or point at infinity\n";
    return Homg::infinity;
  }

  double numerator = vnl_math_sqr (dot(line, point));
  double denominator = (vnl_math_sqr(line.x()) + vnl_math_sqr(line.y())) * vnl_math_sqr(point.w());

  return numerator / denominator;
}

//: Return the distance of a line segment to a line.
//  This is defined as the maximum of the distances of the two endpoints to the line.
double HomgOperator2D::distance_squared(const HomgLineSeg2D& segment, const HomgLine2D& line)
{
  return vnl_math_max(perp_dist_squared(segment.get_point1(), line),
                   perp_dist_squared(segment.get_point2(), line));
}


//: Return distance between line segments.
double HomgOperator2D::distance(const HomgLineSeg2D& ls, const HomgLineSeg2D& ll, double OVERLAP_THRESH)
{
  double norm = vcl_sqrt(ll.get_line().x()*ll.get_line().x()+ll.get_line().y()*ll.get_line().y());
  HomgLine2D lll;
  lll.set(ll.get_line().x()/norm,ll.get_line().y()/norm,ll.get_line().w()/norm);
  double dist1 = ls.get_point1().x()/ls.get_point1().w() * lll.x() +
                 ls.get_point1().y()/ls.get_point1().w() * lll.y() + lll.w();
  double dist2 = ls.get_point2().x()/ls.get_point2().w() * lll.x() +
                 ls.get_point2().y()/ls.get_point2().w() * lll.y() + lll.w();
#ifdef DEBUG
  vcl_cerr << "dist 1 is " <<dist1 << " dist 2 is " <<dist2 << vcl_endl;
#endif
  double dist = (vcl_fabs(dist1) + vcl_fabs(dist2))/2;

  // compute overlap
  // if smaller than OVERLAP_THRESH then reject

  //project ls.point1 and point2 onto ll
  vnl_double_2 p1(ls.get_point1().x()/ls.get_point1().w()-dist1*lll.x(),
                  ls.get_point1().y()/ls.get_point1().w()-dist1*lll.y());

  vnl_double_2 p2(ls.get_point2().x()/ls.get_point2().w()-dist2*(lll.x()),
                  ls.get_point2().y()/ls.get_point2().w()-dist2*(lll.y()));

  vnl_double_2 p3(ll.get_point1().x()/ll.get_point1().w(),
                  ll.get_point1().y()/ll.get_point1().w());

  vnl_double_2 p4(ll.get_point2().x()/ll.get_point2().w(),
                  ll.get_point2().y()/ll.get_point2().w());

  vnl_double_2 v1 = p2 - p1, v2 = p3 - p1, v3 = p4 - p1;
  double r3 = v2(0)/v1(0); if (r3 > 1) r3 = 1; else if (r3 < 0) r3 =0;
  double r4 = v3(0)/v1(0); if (r4 > 1) r4 = 1; else if (r4 < 0) r4 =0;

  double r = vcl_fabs(r3-r4) * v1.two_norm();

  if (r < OVERLAP_THRESH)
    dist = 1000000;

  return dist;
}


//: Return the "Schmid distance" from a point to a line segment.
//  This is the distance to the closest point on the segment, be it endpoint or interior.
// UNTESTED.
double HomgOperator2D::distance_squared (const HomgLineSeg2D& lineseg, const HomgPoint2D& p)
{
  const HomgPoint2D& p1 = lineseg.get_point1();
  const HomgPoint2D& p2 = lineseg.get_point2();

  double p1x = p1[0] / p1[2];
  double p1y = p1[1] / p1[2];

  double p2x = p2[0] / p2[2];
  double p2y = p2[1] / p2[2];

  double dx = p2x - p1x;
  double dy = p2y - p1y;

  double l = vcl_sqrt(dx*dx + dy*dy);

  double px = p[0] / p[2];
  double py = p[1] / p[2];

  double d = ((px - p1x)*dx + (py - p1y)*dy);
  if (d < 0)
    return distance_squared(p, p1);
  if (d > l)
    return distance_squared(p, p2);

  return perp_dist_squared(p, lineseg);
}

// == ANGLES ==

//-----------------------------------------------------------------------------
//: Get the anticlockwise angle between a line and the x axis.
double HomgOperator2D::line_angle(const HomgLine2D& line)
{
  return vcl_atan2(line.y(), line.x());
}

//-----------------------------------------------------------------------------
//: Get the 0 to pi/2 angle between two lines
double HomgOperator2D::abs_angle(const HomgLine2D& line1, const HomgLine2D& line2)
{
  double angle1 = line_angle (line1);
  double angle2 = line_angle (line2);

  double diff = vnl_math_abs(angle2 - angle1);

  if (diff > vnl_math::pi)
    diff -= vnl_math::pi;

  if (diff > vnl_math::pi/2)
    return vnl_math::pi - diff;
  else
    return diff;
}

//-----------------------------------------------------------------------------
//
//: Get the angle between two lines.
// Although homogeneous coordinates are only defined up to scale, here it is
// assumed that a line with homogeneous coordinates (m) is at 180 degrees to
// a line (-m), and this is why the term "oriented_line" is used.  However,
// the overall scale (apart from sign) is not significant.

double HomgOperator2D::angle_between_oriented_lines (const HomgLine2D& line1,
                                                     const HomgLine2D& line2)
{
  double angle1 = line_angle (line1);
  double angle2 = line_angle (line2);

  double diff = angle2 - angle1;

  if (diff > vnl_math::pi)
    return diff - 2.0 * vnl_math::pi;

  if (diff < -vnl_math::pi)
    return diff + 2.0 * vnl_math::pi;

  return diff;
}

// == JOINS/INTERSECTIONS ==

//-----------------------------------------------------------------------------
//
//: Get the line through two points (the cross-product).
//

HomgLine2D HomgOperator2D::join (const HomgPoint2D& point1, const HomgPoint2D& point2)
{
  HomgLine2D answer;
  cross(point1, point2, &answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
//: Get the line through two points (the cross-product).
//  In this case, we assume that the points are oriented, and ensure the cross
// is computed with positive point omegas.

HomgLine2D HomgOperator2D::join_oriented (const HomgPoint2D& point1, const HomgPoint2D& point2)
{
  double x1 = point1.x();
  double y1 = point1.y();
  double w1 = point1.w();
  bool s1 = w1 < 0;

  double x2 = point2.x();
  double y2 = point2.y();
  double w2 = point2.w();
  bool s2 = w2 < 0;

  if (s1 ^ s2)
    return HomgLine2D(-y1 * w2 + w1 * y2, -w1 * x2 + x1 * w2, -x1 * y2 + y1 * x2);
  else
    return HomgLine2D( y1 * w2 - w1 * y2,  w1 * x2 - x1 * w2,  x1 * y2 - y1 * x2);
}

//-----------------------------------------------------------------------------
//
//: Get the intersection point of two lines (the cross-product).
//

HomgPoint2D HomgOperator2D::intersection (const HomgLine2D& line1, const HomgLine2D& line2)
{
  HomgPoint2D answer;
  cross(line1, line2, &answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
//: Get the perpendicular line to line which passes through point.
// Params are line $(a,b,c)$ and point $(x,y,1)$.
// Then the cross product of $(x,y,1)$ and the line's direction $(a,b,0)$,
// called $(p,q,r)$ satisfies
//
//   $ap+bq=0$ (perpendicular condition) and
//
//   $px+qy+r=0$ (incidence condition).

HomgLine2D HomgOperator2D::perp_line_through_point (const HomgLine2D& line,
                                                    const HomgPoint2D& point)
{
  HomgLine2D direction(line.x(), line.y(), 0);
  HomgLine2D answer;
  cross(direction, point, &answer);
  unitize(&answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
//: Get the perpendicular projection of point onto line.
//

HomgPoint2D HomgOperator2D::perp_projection (const HomgLine2D& line,
                                             const HomgPoint2D& point)
{
  HomgLine2D perpline = perp_line_through_point (line, point);
  HomgPoint2D answer;
  cross(line, perpline, &answer);
  unitize(&answer);
  return answer;
}

//: Return the midpoint of the line joining two homogeneous points
HomgPoint2D HomgOperator2D::midpoint (const HomgPoint2D& p1, const HomgPoint2D& p2)
{
  return p1 * (1/(2*p1[2])) + p2*(1/(2*p2[2]));
}

// == FITTING ==

// - Kanatani sect 2.2.2.
static vnl_vector<double> most_orthogonal_vector(const vcl_vector<HomgLine2D>& inpoints)
{
  vnl_scatter_3x3<double> scatter_matrix;

  for (unsigned i = 0; i < inpoints.size(); i++)
    scatter_matrix.add_outer_product(inpoints[i].get_vector());

  return scatter_matrix.minimum_eigenvector().as_ref();
}

static vnl_vector<double> most_orthogonal_vector_svd(const vcl_vector<HomgLine2D>& lines)
{
  vnl_matrix<double> D(lines.size(), 3);

  for (unsigned i = 0; i < lines.size(); i++)
    D.set_row(i, lines[i].get_vector());

  vnl_svd<double> svd(D);
  vcl_cerr << "[movrank " << svd.W() << ']';

  return svd.nullvector();
}

bool lines_to_point_use_svd = false;

//: Intersect a set of 2D lines to find the least-square point of intersection.
// This finds the point $\bf x$ that minimizes $\|\tt L \bf x\|$, where $\tt L$
// is the matrix whose rows are the lines. The current implementation uses the
// vnl_scatter_3x3<double> class from vnl to accumulate and compute the
// nullspace of $\tt L^\top \tt L$.
HomgPoint2D HomgOperator2D::lines_to_point(const vcl_vector<HomgLine2D>& lines)
{
  // ho_triveccam_aspect_lines_to_point
  assert(lines.size() >= 2);

  if (lines_to_point_use_svd)
    return most_orthogonal_vector_svd(lines);
  else
    return most_orthogonal_vector(lines);
}

// == MISCELLANEOUS ==
//
//: Clip line to lineseg.
// The infinite line is clipped against the viewport with
// lower left corner (x0,y0) and upper right corner (x1,y1)

HomgLineSeg2D HomgOperator2D::clip_line_to_lineseg(const HomgLine2D& line,
                                                   double x0, double y0,
                                                   double x1, double y1)
{
  double nx = line.x();
  double ny = line.y();
  double nz = line.w();

  bool intersect_lr = vnl_math_abs(ny) > vnl_math_abs(nx);

  if (intersect_lr) {
    // Clip against verticals
    HomgPoint2D p1(x0 * ny, -(nz + x0 * nx), ny);
    HomgPoint2D p2(x1 * ny, -(nz + x1 * nx), ny);
    return HomgLineSeg2D(p1, p2);
  } else {
    HomgPoint2D p1(-(nz + y0 * ny), y0 * nx, nx);
    HomgPoint2D p2(-(nz + y1 * ny), y1 * nx, nx);
    return HomgLineSeg2D(p1, p2);
  }
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
double HomgOperator2D::CrossRatio(const Homg2D& a, const Homg2D& b, const Homg2D& c, const Homg2D& d)
{
  double x1 = a.x(), y1 = a.y(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), w3 = c.w();
  double x4 = d.x(), y4 = d.y(), w4 = d.w();
  double x = x1 - x2; if (x<0) x = -x; // assuming a != b ;-)
  double y = y1 - y2; if (y<0) y = -y;
  double n = (x>y) ? (x1*w3-x3*w1)*(x2*w4-x4*w2) : (y1*w3-y3*w1)*(y2*w4-y4*w2);
  double m = (x>y) ? (x1*w4-x4*w1)*(x2*w3-x3*w2) : (y1*w4-y4*w1)*(y2*w3-y3*w2);
  if (n == 0 && m == 0)
    vcl_cerr << "CrossRatio not defined: three of the given points coincide\n";
  return n/m;
}

//: Conjugate point of three given collinear points.
// If cross ratio cr is given (default: -1), the generalized conjugate point
// returned is such that ((x1,x2;x3,answer)) = cr.
Homg2D HomgOperator2D::Conjugate(const Homg2D& a, const Homg2D& b, const Homg2D& c, double cr)
// Default for cr is -1.
{
  double x1 = a.x(), y1 = a.y(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), w3 = c.w();
  double kx = x1*w3 - x3*w1, mx = x2*w3 - x3*w2, nx = kx*w2-cr*mx*w1;
  double ky = y1*w3 - y3*w1, my = y2*w3 - y3*w2, ny = ky*w2-cr*my*w1;
  return Homg2D((x2*kx-cr*x1*mx)*ny, (y2*ky-cr*y1*my)*nx, nx*ny);
}
