// Author: Don Hamilton Peter Tu
// Copyright:
// Created: Feb 16 2000
//: 2D homogeneous operations


#include <assert.h>
#include <vcl/vcl_iostream.h>
#include <vcl/vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_scatter_3x3.h>


#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>

#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vnl/vnl_numeric_limits.h> // for infinity

//-----------------------------------------------------------------------------

template <class Type>
vnl_vector<Type> vgl_homg_operators_2d<Type>::get_vector(vgl_homg_point_2d<Type> &p)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(3);
  v.put(0,p.x());
  v.put(1,p.y());
  v.put(2,p.w());
 
  return v;
}

template <class Type>
vnl_vector<Type> vgl_homg_operators_2d<Type>::get_vector(vgl_homg_line_2d<Type> &l)
{
  // make a vnl_vector for the point p

  vnl_vector<Type> v(3);
  v.put(0,p.a());
  v.put(1,p.b());
  v.put(2,p.c());
 

  return v;
}


// -- Cross product of two vgl_homg_point_2d<Type>s
template <class Type>
void vgl_homg_operators_2d<Type>::cross(const vgl_homg_point_2d<Type>& a, const vgl_homg_point_2d<Type>& b, vgl_homg_point_2d<Type>& a_cross_b)
{
  double x1 = a.x();
  double y1 = a.y();
  double w1 = a.w();
  
  double x2 = b.x();
  double y2 = b.y();
  double w2 = b.w();

  a_cross_b.set(y1 * w2 - w1 * y2,
		  w1 * x2 - x1 * w2,
		  x1 * y2 - y1 * x2);
}

//-----------------------------------------------------------------------------
// -- Dot product of two vgl_homg_point_2d<Type>s
template <class Type>
double vgl_homg_operators_2d<Type>::dot(const vgl_homg_point_2d<Type>& a, const vgl_homg_point_2d<Type>& b)
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
// -- Normalize vgl_homg_point_2d<Type> to unit magnitude

template <class Type>
void vgl_homg_operators_2d<Type>::unitize(vgl_homg_point_2d<Type>& a)
{
  double x = a.x();
  double y = a.y();
  double z = a.w();

  double norm = vnl_math::sqr (vnl_math::sqr(x) + 
                               vnl_math::sqr(y) + 
                               vnl_math::sqr(z));
  
  if (norm == 0.0) {
    cerr << "vgl_homg_operators_2d<Type>::unitize() -- Zero length vector\n";
    return;
  }
  
  norm = 1.0/norm;
  a.set(x*norm, y*norm, z*norm);
}

// @{ DISTANCE MEASUREMENTS IN IMAGE COORDINATES @}

// -- Get the square of the 2D distance between the two points.
template <class Type>
double vgl_homg_operators_2d<Type>::distance_squared (const vgl_homg_point_2d<Type>& point1,
					 const vgl_homg_point_2d<Type>& point2)
{
  double x1 = point1.x();
  double y1 = point1.y();
  double z1 = point1.w();

  double x2 = point2.x();
  double y2 = point2.y();
  double z2 = point2.w();

  if (z1 == 0 || z2 == 0) {
    cerr << "vgl_homg_operators_2d<Type>::distance_squared() -- point at infinity";
    return vnl_numeric_limits<Type>::infinity();
  }

  double scale1 = 1.0/z1;
  double scale2 = 1.0/z2;
  
  return (vnl_math::sqr (x1 * scale1 - x2 * scale2) +
	  vnl_math::sqr (y1 * scale1 - y2 * scale2));
}

//-----------------------------------------------------------------------------
//
// -- Get the square of the perpendicular distance to a line.
// This is just the homogeneous form of the familiar 
// @{ $ \frac{a x + b y + c}{\sqrt{a^2+b^2}} $ @}:
// @{ \[ d = \frac{(l^\top p)}{p_z\sqrt{l_x^2 + l_y^2}} \] @}
// If either the point or the line are at infinity an error message is
// printed and Homg::infinity is returned.

template <class Type>
double vgl_homg_operators_2d<Type>::perp_dist_squared (const vgl_homg_point_2d<Type>& point, const vgl_homg_line_2d<Type>& line)
{
  if (line.ideal() || point.ideal()) {
    cerr << "vgl_homg_operators_2d<Type>::perp_dist_squared() -- line or point at infinity";
    // return Homg::infinity;
    return 10000000; // TODO make an infinity for homg operators 
  }
  
  double numerator = vnl_math::sqr (dot(line, point));
  double denominator = (vnl_math::sqr (line.x()) + vnl_math::sqr(line.y())) *
    vnl_math::sqr (point.w());

  return numerator / denominator;
}


// @{ ANGLES @}

//-----------------------------------------------------------------------------
// -- Get the anticlockwise angle between a line and the x axis.
template <class Type>
double vgl_homg_operators_2d<Type>::line_angle(const vgl_homg_line_2d<Type>& line)
{
  return atan2 (line.y(), line.x());
}

//-----------------------------------------------------------------------------
// -- Get the 0 to pi/2 angle between two lines
template <class Type>
double vgl_homg_operators_2d<Type>::abs_angle(const vgl_homg_line_2d<Type>& line1, const vgl_homg_line_2d<Type>& line2)
{
  double angle1 = line_angle (line1);
  double angle2 = line_angle (line2);

  double diff = vnl_math::abs(angle2 - angle1);
  
  if (diff > vnl_math::pi)
    diff -= vnl_math::pi;

  if (diff > vnl_math::pi/2)
    return vnl_math::pi - diff;
  else
    return diff;
}

//-----------------------------------------------------------------------------
//
// -- Get the angle between two lines.  Although homogeneous coordinates are
// only defined up to scale, here it is assumed that a line with homogeneous
// coordinates (m) is at 180 degrees to a line (-m), and this is why the term
// "oriented_line" is used.  However, the overall scale (apart from sign) is
// not significant.
// 

template <class Type>
double vgl_homg_operators_2d<Type>::angle_between_oriented_lines (const vgl_homg_line_2d<Type>& line1,
						     const vgl_homg_line_2d<Type>& line2)
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

// @{ JOINS/INTERSECTIONS @}

//-----------------------------------------------------------------------------
//
// -- Get the line through two points (the cross-product).
// 

template <class Type>
vgl_homg_line_2d<Type> vgl_homg_operators_2d<Type>::join (const vgl_homg_point_2d<Type>& point1, const vgl_homg_point_2d<Type>& point2)
{
  vgl_homg_line_2d<Type> answer;
  cross(point1, point2, answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
// -- Get the line through two points (the cross-product).  In this case, we assume
// that the points are oriented, and ensure the cross is computed with positive point
// omegas.
// 

template <class Type>
vgl_homg_line_2d<Type> vgl_homg_operators_2d<Type>::join_oriented (const vgl_homg_point_2d<Type>& point1, const vgl_homg_point_2d<Type>& point2)
{
  double x1 = point1.x();
  double y1 = point1.y();
  double w1 = point1.w();
  int s1 = w1 < 0;
  
  double x2 = point2.x();
  double y2 = point2.y();
  double w2 = point2.w();
  int s2 = w2 < 0;

  if (s1 ^ s2)
    return vgl_homg_line_2d<Type>(-y1 * w2 + w1 * y2, -w1 * x2 + x1 * w2, -x1 * y2 + y1 * x2);
  else
    return vgl_homg_line_2d<Type>( y1 * w2 - w1 * y2,  w1 * x2 - x1 * w2,  x1 * y2 - y1 * x2);
}

//-----------------------------------------------------------------------------
//
// -- Get the intersection point of two lines (the cross-product).
// 

template <class Type>
vgl_homg_point_2d<Type> vgl_homg_operators_2d<Type>::intersection (const vgl_homg_line_2d<Type>& line1, const vgl_homg_line_2d<Type>& line2)
{
  vgl_homg_point_2d<Type> answer;
  cross(line1, line2, answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
// -- @{ Get the perpendicular line to line which passes through point.
// Params are line $(a,b,c)$ and point $(x,y,1)$.
// Then the cross product of $(x,y,1)$ and the line's direction $(a,b,0)$,
// called $(p,q,r)$ satisfies
// 
//   $ap+bq=0$ (perpendicular condition) and
//   
//   $px+qy+r=0$ (incidence condition).
// @}

template <class Type>
vgl_homg_line_2d<Type> vgl_homg_operators_2d<Type>::perp_line_through_point (const vgl_homg_line_2d<Type>& line,
						    const vgl_homg_point_2d<Type>& point)
{
  vgl_homg_line_2d<Type> direction(line.x(), line.y(), 0);  
  vgl_homg_line_2d<Type> answer;
  cross(direction, point, answer);
  unitize(answer);
  return answer;
}

//-----------------------------------------------------------------------------
//
// -- Get the perpendicular projection of point onto line.
// 

template <class Type>
vgl_homg_point_2d<Type> vgl_homg_operators_2d<Type>::perp_projection (const vgl_homg_line_2d<Type>& line,
					     const vgl_homg_point_2d<Type>& point)
{
  vgl_homg_line_2d<Type> perpline = perp_line_through_point (line, point);
  vgl_homg_point_2d<Type> answer;
  cross(line, perpline, answer);
  unitize(answer);
  return answer;
}

// -- Return the midpoint of the line joining two homogeneous points
template <class Type>
vgl_homg_point_2d<Type> vgl_homg_operators_2d<Type>::midpoint (const vgl_homg_point_2d<Type>& p1, const vgl_homg_point_2d<Type>& p2)
{
  // return p1 * (1/(2*p1[2])) + p2*(1/(2*p2[2]));
  
  double x = p1.x() /(2*p1.w()) + p2.x() / (2*p2.w());
  double y = p1.y() /(2*p1.w()) + p2.y() / (2*p2.w());
  double w = p1.w() /(2*p1.w()) + p2.w() / (2*p2.w());
  
  return vgl_homg_point_2d<Type>(x,y,w);
}

// @{ FITTING @}

// - Kanatani sect 2.2.2.
template <class Type>
vnl_vector<double> vgl_homg_operators_2d<Type>::most_orthogonal_vector(const vcl_vector<vgl_homg_line_2d<Type> >& inpoints)
{
  vnl_scatter_3x3<double> scatter_matrix;
  
  for (int i = 0; i < inpoints.size(); i++)
    scatter_matrix.add_outer_product(get_vector(inpoints[i]));
  
  return vnl_symmetric_eigensystem(scatter_matrix).get_nullvector();
}

#include <vnl/algo/vnl_svd.h>

template <class Type>
vnl_vector<double> vgl_homg_operators_2d<Type>::most_orthogonal_vector_svd(const vcl_vector<vgl_homg_line_2d<Type> >& lines)
{
  vnl_matrix<Type> D(lines.size(), 3);
  
  for (int i = 0; i < lines.size(); i++){
  
    D.set_row(i, get_vector(lines[i]));
  }

  vnl_svd<Type> svd(D);
  cout << "[movrank " << svd.W() << "]";
  
  return svd.nullvector();
}

bool lines_to_point_use_svd = false;

// -- Intersect a set of 2D lines to find the least-square point of intersection.
// @{ This finds the point $\bf x$ that minimizes $\|\tt L \bf x\|$, where $\tt L$ is the matrix whose
// rows are the lines. The current implementation uses the Scatter3x3 class from
// Numerics to accumulate and compute the nullspace of $\tt L^\top \tt L$  @}
template <class Type>
vgl_homg_point_2d<Type> vgl_homg_operators_2d<Type>::lines_to_point(const vcl_vector<vgl_homg_line_2d<Type> >& lines)
{
  // ho_triveccam_aspect_lines_to_point
  assert(lines.size() >= 2);

  if (lines_to_point_use_svd)
    return most_orthogonal_vector_svd(lines);
  else
    return most_orthogonal_vector(lines);
}

// @{ MISCELLANEOUS @}


template <class Type>
double vgl_homg_operators_2d<Type>::perp_distance_squared (const vgl_homg_line_2d<Type>& line, const vgl_homg_point_2d<Type>& point)
{
  cerr << "vgl_homg_operators_2d<Type>::perp_distance_squared should be replaced by perp_dist_squared\n";
  return perp_dist_squared(point, line);
}

//-----------------------------------------------------------------------------
// -- Calculates the crossratio of four collinear points p1, p2, p3 and p4.
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
template <class Type>
double vgl_homg_operators_2d<Type>::CrossRatio(const vgl_homg_point_2d<Type>& a, const vgl_homg_point_2d<Type>& b, const vgl_homg_point_2d<Type>& c, const vgl_homg_point_2d<Type>& d)
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
    cerr << "CrossRatio not defined: three of the given points coincide" << endl;
  return n/m;
}

// -- Conjugate point of three given colinear points.
// If cross ratio cr is given (default: -1), the generalized conjugate point
// returned is such that ((x1,x2;x3,answer)) = cr.
template <class Type>
vgl_homg_point_2d<Type> vgl_homg_operators_2d<Type>::Conjugate(const vgl_homg_point_2d<Type>& a, const vgl_homg_point_2d<Type>& b, const vgl_homg_point_2d<Type>& c, double cr)
// Default for cr is -1.
{
  double x1 = a.x(), y1 = a.y(), w1 = a.w();
  double x2 = b.x(), y2 = b.y(), w2 = b.w();
  double x3 = c.x(), y3 = c.y(), w3 = c.w();
  double kx = x1*w3 - x3*w1, mx = x2*w3 - x3*w2, nx = kx*w2-cr*mx*w1;
  double ky = y1*w3 - y3*w1, my = y2*w3 - y3*w2, ny = ky*w2-cr*my*w1;
  return vgl_homg_point_2d<Type>((x2*kx-cr*x1*mx)*ny, (y2*ky-cr*y1*my)*nx, nx*ny);
}
