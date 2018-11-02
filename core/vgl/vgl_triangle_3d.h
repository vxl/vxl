#ifndef VGL_TRIANGLE_3D_H_
#define VGL_TRIANGLE_3D_H_
//:
// \file
// \brief Some helpful functions when working with triangles
// \author Kieran O'Mahony
// \date 21 June 2007

#include <algorithm>
#include <utility>
#include <cmath>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_point_3d.h>

enum vgl_triangle_3d_intersection_t
{
  None=0,
  Skew,
  Coplanar
};

//: Check for coincident edges of triangles a and b
//  \return a vector of the coincident edges
std::vector<std::pair<unsigned,unsigned> > vgl_triangle_3d_coincident_edges(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3);

//: Check if the given point is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
bool vgl_triangle_3d_test_inside(
  const vgl_point_3d<double>& i_pnt,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3);

//: Check if the given point is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
//  \param coplanar_tolerance is used to dismiss points because they are
//    outside the plane. This doesn't widen the triangle, just thickens it.
bool vgl_triangle_3d_test_inside(const vgl_point_3d<double>& i_pnt,
                                 const vgl_point_3d<double>& p1,
                                 const vgl_point_3d<double>& p2,
                                 const vgl_point_3d<double>& p3,
                                 double coplanar_tolerance );

//: Check if point \a i_pnt is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
//  \note this method uses the less efficient 'angles' method which requires 3 calls to acos()
bool vgl_triangle_3d_test_inside_simple(
  const vgl_point_3d<double>& i_pnt,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3 );


//: Compute the intersection point between the line segment and triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if line intersects triangle
vgl_triangle_3d_intersection_t vgl_triangle_3d_line_intersection(
  const vgl_line_segment_3d<double>& line,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3,
  vgl_point_3d<double>& i_pnt,
  bool ignore_coplanar = false);

//: Compute if the given triangles a and b intersect
//  The triangles are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
//  \return intersection type
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3);

//: Compute the intersection line of the given triangles
//  \see vgl_triangle_3d_triangle_intersection()
//  \note an intersection line is not computed for a coplanar intersection
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3,
  vgl_line_segment_3d<double>& i_line);

//: compute the intersection line of the given triangles
//  \see vgl_triangle_3d_triangle_intersection()
//  \note an intersection line is not computed for a coplanar intersection
//  \retval i_line_point1_edge A number [0-5] indicating which edge of the two triangles
//   point1 of i_line lies on. 0 indicates [a_p1,a_p2], 1 - [a_p2,a_p3], 2 - [a_p3,a_p1],
//   3 - [b_p1,b_p2], 4 - [b_p2,b_p3], 5 - [b_p3,b_p1]
//  \retval i_line_point2_edge. As i_line_point1_edge, but for the other end of the intersection.
//  \note if i_line_point1_edge==i_line_point2_edge, this indicates that due to coplanarity, or
//  some other corner case, there were more than two edges involved in the intersection
//  boundaries. The returned edge is one of those edges.
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3,
  vgl_line_segment_3d<double>& i_line,
  unsigned &i_line_point1_edge,
  unsigned &i_line_point2_edge);

//: Compute the line of intersection of the given triangle and plane
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return intersection type
//  \note an intersection line is not defined (NaN) for a coplanar intersection
vgl_triangle_3d_intersection_t vgl_triangle_3d_plane_intersection(
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3,
  const vgl_plane_3d<double>& i_plane,
  vgl_line_segment_3d<double>& i_line);

//: Compute the longest side of the given triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return length of the longest side
inline double vgl_triangle_3d_longest_side(
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3)
{
  double side_length_max = std::max( (p2 - p1).sqr_length(), (p3 - p2).sqr_length());
  side_length_max = std::max( side_length_max, (p1 - p3).sqr_length());
  return std::sqrt(side_length_max);
}

//: Compute the shortest side of the given triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return length of the longest side
inline double vgl_triangle_3d_shortest_side(
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3)
{
  double side_length_min = std::min( (p2 - p1).sqr_length(), (p3 - p2).sqr_length());
  side_length_min = std::min( side_length_min, (p1 - p3).sqr_length());
  return std::sqrt(side_length_min);
}

//: Compute the closest point on a triangle to a reference point
//  The triangle is represented by its vertices \a p1, \a p2, \a p3.
//  \param q The reference point.
//  \return The closest point on the triangle. This may be inside the triangle, or it may be a point on one of the triangle edges.
vgl_point_3d<double> vgl_triangle_3d_closest_point(
  const vgl_point_3d<double>& q,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3);

//: Compute the distance to the closest point on a triangle from a reference point.
//  The triangle is represented by its vertices \a p1, \a p2, \a p3.
//  \param q The reference point.
//  \return The distance to the closest point on the triangle. (The closest point may be inside the triangle, or it may be a point on one of the triangle edges.)
double vgl_triangle_3d_distance(
  const vgl_point_3d<double>& q,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3);

//: Check if the two triangles are coplanar
//  The triangles are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
bool vgl_triangle_3d_triangle_coplanar(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3);


//=======================================================================
//: Compute the area of a triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
double vgl_triangle_3d_area(
  const vgl_point_3d<double> &p0,
  const vgl_point_3d<double> &p1,
  const vgl_point_3d<double> &p2 );

//=======================================================================
//: Compute the aspect ratio of a triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
double vgl_triangle_3d_aspect_ratio(
  const vgl_point_3d<double> &p0,
  const vgl_point_3d<double> &p1,
  const vgl_point_3d<double> &p2 );

#endif // VGL_TRIANGLE_3D_H_
