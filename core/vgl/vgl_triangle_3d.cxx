#include <limits>
#include "vgl_triangle_3d.h"
//:
// \file
// \brief Some helpful functions when working with triangles
// \author Kieran O'Mahony
// \date 21 June 2007

#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_closest_point.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

// Define a file-scope vgl_nan constant
static const double vgl_nan = std::sqrt(-1.0);
static const double sqrteps = std::sqrt(std::numeric_limits<double>::epsilon());
static const double pi = 3.14159265358979323846;

namespace
{
  //: Create plane through three points. Ignore degeneracy.
  vgl_plane_3d<double> create_plane_and_ignore_degenerate(const vgl_point_3d<double>& p1,
                                                          const vgl_point_3d<double>& p2,
                                                          const vgl_point_3d<double>& p3)
  {
    vgl_plane_3d<double> plane;
    auto *a = reinterpret_cast<double *>(&plane);

    a[0] = p2.y()*p3.z() - p2.z()*p3.y()
         + p3.y()*p1.z() - p3.z()*p1.y()
         + p1.y()*p2.z() - p1.z()*p2.y();

    a[1] = p2.z()*p3.x() - p2.x()*p3.z()
         + p3.z()*p1.x() - p3.x()*p1.z()
         + p1.z()*p2.x() - p1.x()*p2.z();

    a[2] = p2.x()*p3.y() - p2.y()*p3.x()
         + p3.x()*p1.y() - p3.y()*p1.x()
         + p1.x()*p2.y() - p1.y()*p2.x();

    a[3] = p1.x()*(p2.z()*p3.y() - p2.y()*p3.z())
         + p2.x()*(p3.z()*p1.y() - p3.y()*p1.z())
         + p3.x()*(p1.z()*p2.y() - p1.y()*p2.z());
    return plane;
  }
}

//=======================================================================
//: Check for coincident edges of triangles a and b
//  \return a vector of the coincident edges
std::vector<std::pair<unsigned,unsigned> > vgl_triangle_3d_coincident_edges(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3)
{
  std::vector<std::pair<unsigned,unsigned> > coinc_edges;

  //create some convenient arrays for looping
  vgl_point_3d<double> a[3] = {a_p1, a_p2, a_p3};
  vgl_point_3d<double> b[3] = {b_p1, b_p2, b_p3};
  std::pair<unsigned,unsigned> e[3] = { std::make_pair(0,1),
                                       std::make_pair(1,2),
                                       std::make_pair(2,0) };

  // Test each edge j of triangle a against each edge i of triangle b.
  for (unsigned j = 0; j < 3; ++j)
  {
    for (unsigned i = 0; i < 3; ++i)
    {
      //check if one edge is entirely contained within the other and vice versa

      double e1_len = length(a[e[j].first] - a[e[j].second]);
      double b1_dist = length(a[e[j].first] - b[e[i].first]) +
        length(a[e[j].second] - b[e[i].first]);
      double b2_dist = length(a[e[j].first] - b[e[i].second]) +
        length(a[e[j].second] - b[e[i].second]);

      double e2_len = length(b[e[i].first] - b[e[i].second]);
      double a1_dist = length(b[e[i].first] - a[e[j].first]) +
        length(b[e[i].second] - a[e[j].first]);
      double a2_dist = length(b[e[i].first] - a[e[j].second]) +
        length(b[e[i].second] - a[e[j].second]);

      if ((std::fabs(e1_len - b1_dist) < sqrteps &&
           std::fabs(e1_len - b2_dist) < sqrteps) ||
          (std::fabs(e2_len - a1_dist) < sqrteps &&
           std::fabs(e2_len - a2_dist) < sqrteps))
      {
        coinc_edges.emplace_back(j,i);
        break;
      }
    }
  }

  return coinc_edges;
}


//=======================================================================
//: Check if the given point is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
//  \param coplanar_tolerance used to dismiss points because they are
//    outside the plane. This doesn't widen the triangle, just thickens it.
bool vgl_triangle_3d_test_inside(const vgl_point_3d<double>& i_pnt,
                                 const vgl_point_3d<double>& p1,
                                 const vgl_point_3d<double>& p2,
                                 const vgl_point_3d<double>& p3,
                                 double coplanar_tolerance)
{
  // firstly perform some degeneracy checks
  if (collinear(p1,p2,p3))
  { //the triangle is degenerate - its vertices are collinear

    if (p1==p2&&p2==p3&&p1==p3)
    { //all its vertices are the same
      return i_pnt == p1;
    }

    return vgl_line_segment_3d<double>(p1,p2).contains(i_pnt) ||
           vgl_line_segment_3d<double>(p2,p3).contains(i_pnt) ||
           vgl_line_segment_3d<double>(p1,p3).contains(i_pnt);
  }

  // Project to 2d plane, to avoid a degenerate result get
  // the plane normal and identify the largest (abs) x,y or z component
  vgl_plane_3d<double> plane =
    create_plane_and_ignore_degenerate(p1, p2, p3);

  // use Badouel's algorithm (a barycentric method)
  // based on the code & paper found at http://jgt.akpeters.com/papers/MollerTrumbore97/

  //the point needs to be in the triangles plane
  if (vgl_distance(plane,i_pnt) > coplanar_tolerance)
    return false;

  vgl_vector_3d<double> norm = plane.normal();
  norm.set(std::fabs(norm.x()),std::fabs(norm.y()),std::fabs(norm.z()));

  unsigned i1 = 0; // Default is z.
  unsigned i2 = 1;
  if (norm.y()>=norm.x() && norm.y()>=norm.z())
  {
    i2 = 2; // Max component is y
  }
  else if (norm.x()>=norm.y() && norm.x()>=norm.z())
  {
    i1 = 2; // Max component is x
  }

  double point[3] = {i_pnt.x(), i_pnt.y(), i_pnt.z()};
  double vert0[3] = {p1.x(), p1.y(), p1.z()};
  double vert1[3] = {p2.x(), p2.y(), p2.z()};
  double vert2[3] = {p3.x(), p3.y(), p3.z()};

  double beta = 0.0;
  double alpha = 0.0;

  //compute the barycentric vectors....& ignore numerical roundoff errors
  double u0 = (std::fabs(point[i1]) < sqrteps ? 0 : point[i1]) - (std::fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double v0 = (std::fabs(point[i2]) < sqrteps ? 0 : point[i2]) - (std::fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);

  double u1 = (std::fabs(vert1[i1]) < sqrteps ? 0 : vert1[i1]) - (std::fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double u2 = (std::fabs(vert2[i1]) < sqrteps ? 0 : vert2[i1]) - (std::fabs(vert0[i1]) < sqrteps ? 0 : vert0[i1]);
  double v1 = (std::fabs(vert1[i2]) < sqrteps ? 0 : vert1[i2]) - (std::fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);
  double v2 = (std::fabs(vert2[i2]) < sqrteps ? 0 : vert2[i2]) - (std::fabs(vert0[i2]) < sqrteps ? 0 : vert0[i2]);

  // calculate and compare barycentric coordinates
  if (u1 == 0)
  {    // uncommon case
    beta = u0 / u2;
    if (beta < -sqrteps/*0*/ || beta > 1+sqrteps)
      return false;
    alpha = (v0 - beta * v2) / v1;
  }
  else
  {      // common case
    beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
    if (beta < -sqrteps/*0*/ || beta > 1+sqrteps)
      return false;
    alpha = (u0 - beta * u2) / u1;
  }

  return alpha        >=    -sqrteps /*0*/
      && alpha + beta <= 1.0+sqrteps;
}


//=======================================================================
//: Check if the given point is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
bool vgl_triangle_3d_test_inside(const vgl_point_3d<double>& i_pnt,
                                 const vgl_point_3d<double>& p1,
                                 const vgl_point_3d<double>& p2,
                                 const vgl_point_3d<double>& p3)
{
  return vgl_triangle_3d_test_inside(i_pnt, p1, p2, p3, sqrteps);
}


//=======================================================================
//: Check if point \a i_pnt is inside the triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return true if point is inside
//
//  \note this method uses the less efficient 'angles' method which requires 3 calls to acos()
bool vgl_triangle_3d_test_inside_simple(const vgl_point_3d<double>& i_pnt,
                                        const vgl_point_3d<double>& p1,
                                        const vgl_point_3d<double>& p2,
                                        const vgl_point_3d<double>& p3 )
{
  vgl_vector_3d<double> vec1 = normalized(i_pnt - p1);
  vgl_vector_3d<double> vec2 = normalized(i_pnt - p2);
  vgl_vector_3d<double> vec3 = normalized(i_pnt - p3);

  double int_ang = std::acos(dot_product(vec1,vec2)) + std::acos(dot_product(vec2,vec3)) + std::acos(dot_product(vec3,vec1));
  double test_val = std::fabs(int_ang-(2*pi));

  return test_val < sqrteps;
}

//! Are D and E on opposite sides of the plane that touches A, B, C
// \returns Skew if on same side, None if not, and Coplanar if D is on
// plane ABC
static vgl_triangle_3d_intersection_t same_side(
  const vgl_point_3d<double>& A,
  const vgl_point_3d<double>& B,
  const vgl_point_3d<double>& C,
  const vgl_point_3d<double>& D,
  const vgl_point_3d<double>& E)
{
  vgl_vector_3d<double> b = B - A;
  vgl_vector_3d<double> c = C - A;

  vgl_vector_3d<double> n = cross_product(b, c);

  vgl_vector_3d<double> d = D - A;
  double d_dot = dot_product(d, n);
  vgl_vector_3d<double> e = E - A;
  double e_dot = dot_product(e, n);

  if (std::abs(d_dot) < std::sqrt(
                                std::numeric_limits<double>::epsilon()) *
      std::max(1.0e-100, std::max(std::sqrt(A.x()*A.x()+A.y()*A.y()+A.z()*A.z()),
                                std::sqrt(D.x()*D.x()+D.y()*D.y()+D.z()*D.z()) ) ) )
  {
    return Coplanar;
  }

  if (d_dot * e_dot >= 0)
    return Skew;
  else
    return None;
}


//=======================================================================
//: Compute the intersection point between the line segment and triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return intersection type
vgl_triangle_3d_intersection_t vgl_triangle_3d_line_intersection(
  const vgl_line_segment_3d<double>& line,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3,
  vgl_point_3d<double>& i_pnt,
  bool ignore_coplanar/*=false*/)
{
  vgl_point_3d<double> line_p1 = line.point1();
  vgl_point_3d<double> line_p2 = line.point2();

  // perform some degeneracy checks on the line and triangle
  if (line_p1 == line_p2)
  { //the line is degenerate - it has zero length
    if (!ignore_coplanar && vgl_triangle_3d_test_inside(line_p1,p1,p2,p3))
      return Coplanar;

    return None;
  }

  if (collinear(p1,p2,p3))
  { //the triangle is degenerate - it's points are collinear
    if (p1==p2&&p2==p3&&p1==p3)
    { //all its vertices are the same
      return !ignore_coplanar && line.contains(p1) ? Coplanar : None;
    }

    vgl_line_3d_2_points<double> i_line(line_p1,line_p2);
    if ( !ignore_coplanar && (
        ( p1!=p2 && concurrent(vgl_line_3d_2_points<double>(p1,p2), i_line) &&
          vgl_intersection(line,vgl_line_segment_3d<double>(p1,p2),i_pnt) ) ||
        ( p2!=p3 && concurrent(vgl_line_3d_2_points<double>(p2,p3), i_line) &&
          vgl_intersection(line,vgl_line_segment_3d<double>(p2,p3),i_pnt) ) ||
        ( p1!=p3 && concurrent(vgl_line_3d_2_points<double>(p1,p3), i_line) &&
          vgl_intersection(line,vgl_line_segment_3d<double>(p1,p3),i_pnt) ) ) )
    {
      return Coplanar;
    }

    return None;
  }

  vgl_triangle_3d_intersection_t rv1 = same_side(line.point1(), p1, p2, p3, line.point2());
  if (rv1 == None) return None;

  vgl_triangle_3d_intersection_t rv2 = same_side(line.point1(), p2, p3, p1, line.point2());
  if (rv2 == None) return None;

  vgl_triangle_3d_intersection_t rv3 = same_side(line.point1(), p3, p1, p2, line.point2());
  if (rv3 == None) return None;

  if (rv1 == Coplanar || rv2 == Coplanar || rv3==Coplanar)
  {
    if (ignore_coplanar)
      return None;
    // coplanar line - uncommon case

    // check each triangle edge.
    // behaviour is to return the first found intersection point
    vgl_line_3d_2_points<double> i_line(line_p1,line_p2);
    vgl_line_segment_3d<double> edge1(p1,p2);

    vgl_point_3d<double> test_pt;
    if (concurrent(vgl_line_3d_2_points<double>(p1,p2),i_line) &&
        vgl_intersection(edge1,line,test_pt))
    {
      i_pnt = test_pt;
      return Coplanar;
    }
    vgl_line_segment_3d<double> edge2(p1,p3);
    if (concurrent(vgl_line_3d_2_points<double>(p1,p3),i_line) &&
        vgl_intersection(edge2,line,test_pt))
    {
      i_pnt = test_pt;
      return Coplanar;
    }
    vgl_line_segment_3d<double> edge3(p2,p3);
    if (concurrent(vgl_line_3d_2_points<double>(p2,p3),i_line) &&
        vgl_intersection(edge3,line,test_pt))
    {
      i_pnt = test_pt;
      return Coplanar;
    }

    //special case of line completely contained within the triangle
    if (vgl_triangle_3d_test_inside(line_p2, p1, p2, p3))
    {
      i_pnt.set(vgl_nan, vgl_nan, vgl_nan);
      return Coplanar;
    }

    return None;
  }

  if (same_side(p1, p2, p3, line_p1, line_p2) == Skew)
    return None;

  i_pnt = vgl_intersection(vgl_line_3d_2_points<double>(line_p1, line_p2),
                           vgl_plane_3d<double>(p1, p2, p3) );
  return Skew;
}


#ifndef UINT_MAX
#define UINT_MAX 0xffffffffU
#endif
namespace
{
  static const unsigned calc_edge_index_lookup[8] = {UINT_MAX, 0, 2, 0, UINT_MAX, 1, 2, 1};
  //: Given the [0,2] index of two vertices, in either order, return the edge index [0,2]
  // E.g. between vertices 2 and 0 is edge 2.
  // Use precalculated list lookup, probably fastest.
  inline unsigned calc_edge_index(unsigned v, unsigned w)
  {
    unsigned lookup = v*3u + w;
    assert (lookup < 8);
    unsigned edge = calc_edge_index_lookup[lookup];
    assert (edge < 3);
    return edge;
  }
}
//=======================================================================
//: Compute the intersection line of the given triangles
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
  unsigned &i_line_point2_edge
 )
{
  // triangle intersection algorithm based on code & paper
  // found at http://jgt.akpeters.com/papers/Moller97/

  //sanity check for degenerate triangles
  if (collinear(a_p1,a_p2,a_p3))
  {
    if (a_p1 == a_p2 && a_p2==a_p3) // if it has degenerated to a single point
    {
      if (vgl_triangle_3d_test_inside(a_p1,b_p1,b_p2,b_p3))
      {
        i_line_point1_edge = i_line_point2_edge = 0;
        i_line.set(a_p1,a_p1);
        return Coplanar;
      }
      return None;
    }

    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    unsigned tmp_iline_edges[2];
    unsigned n_tmp_iline_edges = 0;
    if ( a_p1 != a_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1,b_p2,b_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 0;
        return ret;
      }
      tmp_iline_edges[0] = 0;
      n_tmp_iline_edges = 1;
    }
    if ( a_p2 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 1;
        return ret;
      }
      tmp_iline_edges[n_tmp_iline_edges] = 1;
      n_tmp_iline_edges++;
    }
    if ( a_p1 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 2;
        return ret;
      }
      if (n_tmp_iline_edges >= 2)
      { // All edges intersect - return repeated edge to indicate problem
        i_line_point1_edge = i_line_point2_edge = 0;
        return ret;
      }
      tmp_iline_edges[n_tmp_iline_edges] = 2;
      n_tmp_iline_edges++;
    }
    if (!n_tmp_iline_edges) return None; // Found no edges intersecting with triangle
    if (n_tmp_iline_edges == 1) // Found one edge intersecting with triangle
    {
      i_line_point1_edge = i_line_point2_edge = tmp_iline_edges[0];
      return Skew;
    }
    // Found two edges intersecting with triangle
    i_line_point1_edge = tmp_iline_edges[0];
    i_line_point2_edge = tmp_iline_edges[1];
    return Skew;
  }
  if (collinear(b_p1,b_p2,b_p3))
  {
    if (b_p1 == b_p2 && b_p2==b_p3 && b_p1 == b_p3)
    {
      if (vgl_triangle_3d_test_inside(b_p1,a_p1,a_p2,a_p3))
      {
        i_line_point1_edge = i_line_point2_edge = 3;
        i_line.set(b_p1,b_p1);
        return Coplanar;
      }
      return None;
    }

    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    unsigned tmp_iline_edges[2];
    unsigned n_tmp_iline_edges = 0;
    if (b_p1 != b_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p2), a_p1,a_p2,a_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 3;
        return ret;
      }
      tmp_iline_edges[0] = 3;
      n_tmp_iline_edges = 1;
    }
    if ( b_p2 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p2,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 4;
        return ret;
      }
      tmp_iline_edges[n_tmp_iline_edges] = 4;
      n_tmp_iline_edges++;
    }
    if ( b_p1 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None )
    {// half-degenerate tri_b behaves as line. Find line intersection
      i_line.set(i_pnt,i_pnt);
      if (ret == Coplanar)
      {
        i_line_point1_edge = i_line_point2_edge = 5;
        return ret;
      }
      if (n_tmp_iline_edges >= 2)
      { // All edges intersect - return repeated edge to indicate problem
        i_line_point1_edge = i_line_point2_edge = 3;
        return ret;
      }
      tmp_iline_edges[n_tmp_iline_edges] = 5;
      n_tmp_iline_edges++;
    }
    if (!n_tmp_iline_edges) return None; // Found no edges intersecting with triangle
    if (n_tmp_iline_edges == 1) // Found one edge intersecting with triangle
    {
      i_line_point1_edge = i_line_point2_edge = tmp_iline_edges[0];
      return Skew;
    }
    // Found two edges intersecting with triangle
    i_line_point1_edge = tmp_iline_edges[0];
    i_line_point2_edge = tmp_iline_edges[1];
    return Skew;
  }

  //computing intersection of triangles a and b

  vgl_vector_3d<double> edge1,edge2;
  vgl_vector_3d<double> a_norm, b_norm, int_line;
  //vgl_vector_3d<double> int_line;
  double a_d, b_d;
  double d_b[3], d_a[3]; // distance of corner [1,2,3] of tri b to plane of tri a, same for tri_a
  double d_b1d_b2, d_b1d_b3, d_a1d_a2, d_a1d_a3;

  double p_a[3];
  double p_b[3];
  bool coplanar = false;

  double TRI_TRI_EPS = 1000000*std::numeric_limits<double>::epsilon();

  //Firstly check if each triangle intersects
  // the plane of the other

  //construct plane equation of triangle a
  edge1 = a_p2 - a_p1;
  edge2 = a_p3 - a_p1;

  a_norm = normalized(cross_product(edge1, edge2));
  a_d = -( a_norm.x()*a_p1.x() + a_norm.y()*a_p1.y() +a_norm.z()*a_p1.z() );
  //vgl_plane_3d<double> a_plane(a_p1,a_p2,a_p3);

  // get signed distance of triangle b to triangle a's plane
  d_b[0] = ( a_norm.x()*b_p1.x() + a_norm.y()*b_p1.y() + a_norm.z()*b_p1.z() ) + a_d;
  d_b[1] = ( a_norm.x()*b_p2.x() + a_norm.y()*b_p2.y() + a_norm.z()*b_p2.z() ) + a_d;
  d_b[2] = ( a_norm.x()*b_p3.x() + a_norm.y()*b_p3.y() + a_norm.z()*b_p3.z() ) + a_d;
#if 0
  d_b[0] = (a_plane.nx()*b_p1.x() + a_plane.ny()*b_p1.y() + a_plane.nz()*b_p1.z() ) + a_plane.d();
  d_b[1] = (a_plane.nx()*b_p2.x() + a_plane.ny()*b_p2.y() + a_plane.nz()*b_p2.z() ) + a_plane.d();
  d_b[2] = (a_plane.nx()*b_p3.x() + a_plane.ny()*b_p3.y() + a_plane.nz()*b_p3.z() ) + a_plane.d();
#endif // 0

  // coplanarity robustness check
  if (std::fabs(d_b[0]) < TRI_TRI_EPS) d_b[0] = 0.0;
  if (std::fabs(d_b[1]) < TRI_TRI_EPS) d_b[1] = 0.0;
  if (std::fabs(d_b[2]) < TRI_TRI_EPS) d_b[2] = 0.0;

  d_b1d_b2 = d_b[0]*d_b[1];
  d_b1d_b3 = d_b[0]*d_b[2];

  // all distances same sign => no intersection
  if (d_b1d_b2 > 0 && d_b1d_b3 > 0)
  {
    return None;
  }

  //construct plane equation of triangle b
  edge1 = b_p2 - b_p1;
  edge2 = b_p3 - b_p1;

  b_norm = normalized(cross_product(edge1,edge2));
  b_d = -( b_norm.x()*b_p1.x() + b_norm.y()*b_p1.y() + b_norm.z()*b_p1.z() );
  //vgl_plane_3d<double> b_plane(b_p1,b_p2,b_p3);

  // get signed distance of triangle a to triangle b's plane
  d_a[0] = ( b_norm.x()*a_p1.x() + b_norm.y()*a_p1.y() + b_norm.z()*a_p1.z() ) + b_d;
  d_a[1] = ( b_norm.x()*a_p2.x() + b_norm.y()*a_p2.y() + b_norm.z()*a_p2.z() ) + b_d;
  d_a[2] = ( b_norm.x()*a_p3.x() + b_norm.y()*a_p3.y() + b_norm.z()*a_p3.z() ) + b_d;
#if 0
  d_a[0] = (b_plane.nx()*a_p1.x() + b_plane.ny()*a_p1.y() + b_plane.nz()*a_p1.z() ) + b_plane.d();
  d_a[1] = (b_plane.nx()*a_p2.x() + b_plane.ny()*a_p2.y() + b_plane.nz()*a_p2.z() ) + b_plane.d();
  d_a[2] = (b_plane.nx()*a_p3.x() + b_plane.ny()*a_p3.y() + b_plane.nz()*a_p3.z() ) + b_plane.d();
#endif // 0

  // coplanarity robustness check
  if (std::fabs(d_a[0]) < TRI_TRI_EPS) d_a[0] = 0.0;
  if (std::fabs(d_a[1]) < TRI_TRI_EPS) d_a[1] = 0.0;
  if (std::fabs(d_a[2]) < TRI_TRI_EPS) d_a[2] = 0.0;

  d_a1d_a2 = d_a[0]*d_a[1];
  d_a1d_a3 = d_a[0]*d_a[2];

  // all distances same sign => no intersection
  if (d_a1d_a2 > 0 && d_a1d_a3 > 0)
  {
    return None;
  }

  // Now we know triangles contain
  // the line of their planes intersection.
  // So...compute each triangles interval of the intersection
  // line and determine if they overlap i.e. if the triangles intersect

  // compute direction of intersection line
  int_line = cross_product(a_norm,b_norm);
  //int_line = cross_product(a_plane.normal(),b_plane.normal());

  // largest component of int_line?
  // to get a simplified 2D projection
  int_line.set(std::fabs(int_line.x()),std::fabs(int_line.y()),std::fabs(int_line.z()));

  if (int_line.y()>=int_line.x() && int_line.y()>=int_line.z())
  { // Max component is y
    p_a[0] = a_p1.y();
    p_a[1] = a_p2.y();
    p_a[2] = a_p3.y();

    p_b[0] = b_p1.y();
    p_b[1] = b_p2.y();
    p_b[2] = b_p3.y();
  }
  else if (int_line.x()>=int_line.y() && int_line.x()>=int_line.z())
  { // Max component is x
    p_a[0] = a_p1.x();
    p_a[1] = a_p2.x();
    p_a[2] = a_p3.x();

    p_b[0] = b_p1.x();
    p_b[1] = b_p2.x();
    p_b[2] = b_p3.x();
  }
  else { // Max component is z
    p_a[0] = a_p1.z();
    p_a[1] = a_p2.z();
    p_a[2] = a_p3.z();

    p_b[0] = b_p1.z();
    p_b[1] = b_p2.z();
    p_b[2] = b_p3.z();
  }

  int a_ival[3] = {0,1,2}; // re-ordering of tri_a, s.t.
                           // a_ival[1,2] are on one side, and a_ival[0] on the plane or other side.
                           // or a_ival[0] one one side and a_ival[1,2) on the plne or other side.
  // compute interval for triangle a
  if (d_a1d_a2 > 0) //a1, a2 on same side of b's plane, a3 on the plane or other side
  {
    a_ival[0] = 2;
    a_ival[1] = 0;
    a_ival[2] = 1;
  }
  else if (d_a1d_a3 > 0) //a1, a3 on same side of b's plane, a2 on the plane or other side
  {
    a_ival[0] = 1;
    a_ival[1] = 0;
    a_ival[2] = 2;
  }
  else if (d_a[1]*d_a[2] > 0 || d_a[0] != 0) //a2, a3 on same side of b's plane, a1 on other side
  {
    a_ival[0] = 0;
    a_ival[1] = 1;
    a_ival[2] = 2;
  }
  else if (d_a[1] != 0) // a2 on one side of the plane, and a1, a3 on the plane
  {
    a_ival[0] = 1;
    a_ival[1] = 0;
    a_ival[2] = 2;
  }
  else if (d_a[2] != 0) // a3 on one side of the plane, and a1, a2 on the plane
  {
    a_ival[0] = 2;
    a_ival[1] = 0;
    a_ival[2] = 1;
  }
  else
  {
    // triangles are coplanar
    coplanar = true;
  }

  int b_ival[3] = {0,1,2}; // see a_ival for description
  if (!coplanar)
  {
    // compute interval for triangle b
    if (d_b1d_b2 > 0) //b1, b2 on same side of a's plane, b3 on the other side
    {
      b_ival[0] = 2;
      b_ival[1] = 0;
      b_ival[2] = 1;
    }
    else if (d_b1d_b3 > 0) //b1, b3 on same side of a's plane, b2 on the other side
    {
      b_ival[0] = 1;
      b_ival[1] = 0;
      b_ival[2] = 2;
    }
    else if (d_b[1]*d_b[2] > 0 || d_b[0] != 0)
    {
      b_ival[0] = 0;
      b_ival[1] = 1;
      b_ival[2] = 2;
    }
    else if (d_b[1] != 0)
    {
      b_ival[0] = 1;
      b_ival[1] = 0;
      b_ival[2] = 2;
    }
    else if (d_b[2] != 0)
    {
      b_ival[0] = 2;
      b_ival[1] = 0;
      b_ival[2] = 1;
    }
    else
    {
      coplanar = true;
    }
  }

  //special case when triangles are coplanar
  if (coplanar)
  {
    //check if they intersect in their common plane
    vgl_point_3d<double> i_pnt1, i_pnt2, i_pnt3;
    bool isect1 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p1,a_p2), b_p1, b_p2, b_p3, i_pnt1) != None;
    bool isect2 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p2,a_p3), b_p1, b_p2, b_p3, i_pnt2) != None;
    bool isect3 = vgl_triangle_3d_line_intersection(
      vgl_line_segment_3d<double>(a_p3,a_p1), b_p1, b_p2, b_p3, i_pnt3) != None;

    if ( isect1 || isect2 || isect3 )
    {
      vgl_point_3d<double> i_line_point1, i_line_point2;
      if (isect1)
      {
        i_line_point1 = i_pnt1;
        i_line_point1_edge = i_line_point2_edge = 0; // Set repeated edges to indicate incomplete answer
      }
      else
      {
        if (isect2)
        {
          i_line_point1 = i_pnt2;
          i_line_point1_edge = i_line_point2_edge = 1;
        }
        else
        {
          i_line_point1 = i_pnt3;
          i_line_point1_edge = i_line_point2_edge = 2;
        }
      }
      // try and get extent of intersection as best as possible
      if (isect1 && isect2)
        i_line_point2 = i_pnt2;
      else if ((isect1 || isect2) && isect3)
        i_line_point2 = i_pnt3;
      else
      {
        if (isect1)
          i_line_point2 = i_pnt1;
        else
        {
          if (isect2)
            i_line_point2 = i_pnt2;
          else
            i_line_point2 = i_pnt3;
        }
      }
      i_line.set( i_line_point1, i_line_point2);
      return Coplanar;
    }

    // finally, test if triangle a is totally contained in triangle b or vice versa
    if (vgl_triangle_3d_test_inside(a_p1, b_p1, b_p2, b_p3))
    {
      i_line_point1_edge = i_line_point2_edge = 0;
      i_line.set(a_p1, a_p3);
      return Coplanar;
    }

    if (vgl_triangle_3d_test_inside(b_p1, a_p1, a_p2, a_p3))
    {
      i_line_point1_edge = i_line_point2_edge = 3;
      i_line.set(b_p1, b_p3);
      return Coplanar;
    }

    return None;
  }

  vgl_point_3d<double> i_pnts[4];
  double isect_a[2]; // selected_direction positions of start and end of intersection line in tri_a's p_a coords,
  //intersection line interval for triangle a
  double tmp = d_a[a_ival[0]]/(d_a[a_ival[0]]-d_a[a_ival[1]]); // fraction along edge a_ival[0,1] to plane_b's intersection.
  isect_a[0] = p_a[a_ival[0]] + (p_a[a_ival[1]] - p_a[a_ival[0]])*tmp;
  vgl_point_3d<double> a_vs[] = {a_p1,a_p2,a_p3};
  vgl_vector_3d<double> diff = a_vs[a_ival[1]] - a_vs[a_ival[0]];
  diff *= tmp;
  i_pnts[0] = a_vs[a_ival[0]] + diff ; // 3D pos of start of intersection of tri_a and plane_b, on edge a[a_ival[0,1]]

  tmp = d_a[a_ival[0]]/(d_a[a_ival[0]]-d_a[a_ival[2]]);
  isect_a[1] = p_a[a_ival[0]] + (p_a[a_ival[2]] - p_a[a_ival[0]])*tmp;
  diff = a_vs[a_ival[2]] - a_vs[a_ival[0]];
  diff *= tmp;
  i_pnts[1] = a_vs[a_ival[0]] + diff; // 3D pos of end of intersection of tri_a and plane_b, on edge a[a_ival[0,2]]

  double isect_b[2]; // selected_direction positions of start and end of intersection line in tri_b's p_b coords,
  //intersection line interval for triangle b
  tmp = d_b[b_ival[0]]/(d_b[b_ival[0]] - d_b[b_ival[1]]);
  isect_b[0] = p_b[b_ival[0]] + (p_b[b_ival[1]] - p_b[b_ival[0]])*tmp;
  vgl_point_3d<double> b_vs[] = {b_p1,b_p2,b_p3};
  diff = b_vs[b_ival[1]] - b_vs[b_ival[0]];
  diff *= tmp;
  i_pnts[2] = b_vs[b_ival[0]] + diff; // 3D pos of start of intersection of tri_b and plane_a, on edge b[b_ival[0,1]]

  tmp = d_b[b_ival[0]]/(d_b[b_ival[0]]-d_b[b_ival[2]]);
  isect_b[1] = p_b[b_ival[0]] + (p_b[b_ival[2]] - p_b[b_ival[0]])*tmp;
  diff = b_vs[b_ival[2]] - b_vs[b_ival[0]];
  diff *= tmp;
  i_pnts[3] = b_vs[b_ival[0]] + diff; // 3D pos of end of intersection of tri_b and plane_a, on edge b[b_ival[0,1]]

  unsigned smallest1 = 0;
  if (isect_a[0] > isect_a[1])
  {
    std::swap(isect_a[0], isect_a[1]);
    smallest1 = 1;
  } // Now isect_a[0] <= isect_a[1]
  unsigned smallest2 = 0;
  if (isect_b[0] > isect_b[1])
  {
    std::swap(isect_b[0], isect_b[1]);
    smallest2 = 1;
  } // Now isect_b[0] <= isect_b[1]

  if (isect_a[1] < isect_b[0] || isect_b[1] < isect_a[0])
  {
    return None; //no intersection
  }

  unsigned i_pt1,i_pt2;
  //find the correct intersection line
  if (isect_b[0]<isect_a[0])
  {
    if (smallest1==0)
    {
      i_pt1 = 0;
      i_line_point1_edge = calc_edge_index(a_ival[0], a_ival[1]);
    }
    else
    {
      i_pt1 = 1;
      i_line_point1_edge = calc_edge_index(a_ival[0], a_ival[2]);
    }

    if (isect_b[1]<isect_a[1])
    {
      if (smallest2==0)
      {
        i_pt2 = 3;
        i_line_point2_edge = calc_edge_index(b_ival[0], b_ival[2]) + 3;
      }
      else
      {
        i_pt2 = 2;
        i_line_point2_edge = calc_edge_index(b_ival[0], b_ival[1]) + 3;
      }
    }
    else
    {
      if (smallest1==0)
      {
        i_pt2 = 1;
        i_line_point2_edge = calc_edge_index(a_ival[0], a_ival[2]);
      }
      else
      {
        i_pt2 = 0;
        i_line_point2_edge = calc_edge_index(a_ival[0], a_ival[1]);
      }
    }
  }
  else
  {
    if (smallest2==0)
    {
      i_pt1 = 2;
      i_line_point1_edge = calc_edge_index(b_ival[0], b_ival[1]) + 3;
    }
    else
    {
      i_pt1 = 3;
      i_line_point1_edge = calc_edge_index(b_ival[0], b_ival[2]) + 3;
    }

    if (isect_b[1]>isect_a[1])
    {
      if (smallest1==0)
      {
        i_pt2 = 1;
        i_line_point2_edge = calc_edge_index(a_ival[0], a_ival[2]);
      }
      else
      {
        i_pt2 = 0;
        i_line_point2_edge = calc_edge_index(a_ival[0], a_ival[1]);
      }
    }
    else
    {
      if (smallest2==0)
      {
        i_pt2 = 3;
        i_line_point2_edge = calc_edge_index(b_ival[0], b_ival[2]) + 3;
      }
      else
      {
        i_pt2 = 2;
        i_line_point2_edge = calc_edge_index(b_ival[0], b_ival[1]) + 3;
      }
    }
  }

  i_line.set(i_pnts[i_pt1],i_pnts[i_pt2]);
  return Skew;
}


//=======================================================================
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
  vgl_line_segment_3d<double>& i_line)
{
  unsigned iline_p1, iline_p2;
  return vgl_triangle_3d_triangle_intersection(
  a_p1, a_p2, a_p3,
  b_p1, b_p2, b_p3,
  i_line, iline_p1, iline_p2);
}

//=======================================================================
//: Compute if the given triangles a and b intersect
//  The triangle are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
//  \return intersection type
vgl_triangle_3d_intersection_t vgl_triangle_3d_triangle_intersection(
  const vgl_point_3d<double>& a_p1,
  const vgl_point_3d<double>& a_p2,
  const vgl_point_3d<double>& a_p3,
  const vgl_point_3d<double>& b_p1,
  const vgl_point_3d<double>& b_p2,
  const vgl_point_3d<double>& b_p3)
{
  // triangle intersection algorithm based on code & paper
  // found at http://jgt.akpeters.com/papers/Moller97/

  //sanity check for degenerate triangles
  if (collinear(a_p1,a_p2,a_p3))
  {
    if (a_p1 == a_p2 && a_p2==a_p3 && a_p1 == a_p3)
    {
      if (vgl_triangle_3d_test_inside(a_p1,b_p1,b_p2,b_p3))
        return Coplanar;
      return None;
    }

    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if ( ( a_p1 != a_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
         ( a_p2 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) ||
         ( a_p1 != a_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p3), b_p1,b_p2,b_p3,i_pnt)) != None ) )
      return ret;

    return None;
  }
  if (collinear(b_p1,b_p2,b_p3))
  {
    if (b_p1 == b_p2 && b_p2==b_p3 && b_p1 == b_p3)
    {
      if (vgl_triangle_3d_test_inside(b_p1,a_p1,a_p2,a_p3))
        return Coplanar;
      return None;
    }

    vgl_triangle_3d_intersection_t ret = None;
    vgl_point_3d<double> i_pnt;
    if ( ( b_p1 != b_p2 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p2), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
         ( b_p2 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p2,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) ||
         ( b_p1 != b_p3 && (ret = vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(b_p1,b_p3), a_p1,a_p2,a_p3,i_pnt)) != None ) )
      return ret;

    return None;
  }

  //computing intersection of triangles a and b

  vgl_vector_3d<double> edge1,edge2;
  vgl_vector_3d<double> a_norm, b_norm, int_line;
//  vgl_vector_3d<double> int_line;

  double a_d, b_d;
  double d_b1, d_b2, d_b3, d_a1, d_a2, d_a3;
  double isect1[2], isect2[2];
  double d_b1d_b2, d_b1d_b3, d_a1d_a2, d_a1d_a3;

  double p_a1, p_a2, p_a3;
  double p_b1, p_b2, p_b3;
  bool coplanar = false;

  double a=0.0, b=0.0, c=0.0, x0=0.0, x1=0.0; // although variables are safely initialised further down,
  double d=0.0, e=0.0, f=0.0, y0=0.0, y1=0.0; // these "=0.0" silence the compiler
  double xx, yy, xxyy, tmp;
  double TRI_TRI_EPS = 100000*std::numeric_limits<double>::epsilon();

  //Firstly check if each triangle intersects
  // the plane of the other

  //construct plane equation of triangle a
  edge1 = a_p2 - a_p1;
  edge2 = a_p3 - a_p1;

  a_norm = normalized(cross_product(edge1, edge2));

  a_d = -( a_norm.x()*a_p1.x() + a_norm.y()*a_p1.y() +a_norm.z()*a_p1.z() );
  //vgl_plane_3d<double> a_plane(a_p1,a_p2,a_p3);

  // get signed distance of triangle b to triangle a's plane
  d_b1 = ( a_norm.x()*b_p1.x() + a_norm.y()*b_p1.y() + a_norm.z()*b_p1.z() ) + a_d;
  d_b2 = ( a_norm.x()*b_p2.x() + a_norm.y()*b_p2.y() + a_norm.z()*b_p2.z() ) + a_d;
  d_b3 = ( a_norm.x()*b_p3.x() + a_norm.y()*b_p3.y() + a_norm.z()*b_p3.z() ) + a_d;
#if 0
  d_b1 = (a_plane.nx()*b_p1.x() + a_plane.ny()*b_p1.y() + a_plane.nz()*b_p1.z() ) + a_plane.d();
  d_b2 = (a_plane.nx()*b_p2.x() + a_plane.ny()*b_p2.y() + a_plane.nz()*b_p2.z() ) + a_plane.d();
  d_b3 = (a_plane.nx()*b_p3.x() + a_plane.ny()*b_p3.y() + a_plane.nz()*b_p3.z() ) + a_plane.d();
#endif // 0

  // coplanarity robustness check
  if (std::fabs(d_b1) < TRI_TRI_EPS) d_b1 = 0.0;
  if (std::fabs(d_b2) < TRI_TRI_EPS) d_b2 = 0.0;
  if (std::fabs(d_b3) < TRI_TRI_EPS) d_b3 = 0.0;

  d_b1d_b2 = d_b1*d_b2;
  d_b1d_b3 = d_b1*d_b3;

  // all distances same sign => no intersection
  if (d_b1d_b2 > 0 && d_b1d_b3 > 0)
  {
    return None;
  }

  //construct plane equation of triangle b
  edge1 = b_p2 - b_p1;
  edge2 = b_p3 - b_p1;

  b_norm = normalized(cross_product(edge1,edge2));

  b_d = -( b_norm.x()*b_p1.x() + b_norm.y()*b_p1.y() + b_norm.z()*b_p1.z() );
  //vgl_plane_3d<double> b_plane(b_p1,b_p2,b_p3);

  // get signed distance of triangle a to triangle b's plane
  d_a1 = ( b_norm.x()*a_p1.x() + b_norm.y()*a_p1.y() + b_norm.z()*a_p1.z() ) + b_d;
  d_a2 = ( b_norm.x()*a_p2.x() + b_norm.y()*a_p2.y() + b_norm.z()*a_p2.z() ) + b_d;
  d_a3 = ( b_norm.x()*a_p3.x() + b_norm.y()*a_p3.y() + b_norm.z()*a_p3.z() ) + b_d;
#if 0
  d_a1 = (b_plane.nx()*a_p1.x() + b_plane.ny()*a_p1.y() + b_plane.nz()*a_p1.z() ) + b_plane.d();
  d_a2 = (b_plane.nx()*a_p2.x() + b_plane.ny()*a_p2.y() + b_plane.nz()*a_p2.z() ) + b_plane.d();
  d_a3 = (b_plane.nx()*a_p3.x() + b_plane.ny()*a_p3.y() + b_plane.nz()*a_p3.z() ) + b_plane.d();
#endif // 0

  // coplanarity robustness check
  if (std::fabs(d_a1) < TRI_TRI_EPS) d_a1 = 0.0;
  if (std::fabs(d_a2) < TRI_TRI_EPS) d_a2 = 0.0;
  if (std::fabs(d_a3) < TRI_TRI_EPS) d_a3 = 0.0;

  d_a1d_a2 = d_a1*d_a2;
  d_a1d_a3 = d_a1*d_a3;

  // all distances same sign => no intersection
  if (d_a1d_a2 > 0 && d_a1d_a3 > 0)
  {
    return None;
  }

  // Now know triangles contain
  // the line of their planes intersection.
  // So...compute each triangles interval of the intersection
  // line and determine if they overlap i.e. if the triangles intersect

  // compute direction of intersection line
  int_line = cross_product(a_norm,b_norm);
  //int_line = cross_product(a_plane.normal(),b_plane.normal());

  // largest component of int_line?
  // to get a simplified 2D projection
  int_line.set(std::fabs(int_line.x()),std::fabs(int_line.y()),std::fabs(int_line.z()));

  if (int_line.y()>=int_line.x() && int_line.y()>=int_line.z())
  { // Max component is y
    p_a1 = a_p1.y();
    p_a2 = a_p2.y();
    p_a3 = a_p3.y();

    p_b1 = b_p1.y();
    p_b2 = b_p2.y();
    p_b3 = b_p3.y();
  }
  else if (int_line.x()>=int_line.y() && int_line.x()>=int_line.z())
  { // Max component is x
    p_a1 = a_p1.x();
    p_a2 = a_p2.x();
    p_a3 = a_p3.x();

    p_b1 = b_p1.x();
    p_b2 = b_p2.x();
    p_b3 = b_p3.x();
  }
  else { // Max component is z
    p_a1 = a_p1.z();
    p_a2 = a_p2.z();
    p_a3 = a_p3.z();

    p_b1 = b_p1.z();
    p_b2 = b_p2.z();
    p_b3 = b_p3.z();
  }

  // compute interval for triangle a
  if (d_a1d_a2 > 0) //a1, a2 on same side of b's plane, a3 on the other side
  {
    a = p_a3;
    b = (p_a1-p_a3)*d_a3;
    c = (p_a2-p_a3)*d_a3;

    x0 = d_a3-d_a1;
    x1 = d_a3-d_a2;
  }
  else if (d_a1d_a3 > 0) //a1, a3 on same side of b's plane, a2 on the other side
  {
    a = p_a2;
    b = (p_a1-p_a2)*d_a2;
    c = (p_a3-p_a2)*d_a2;

    x0 = d_a2-d_a1;
    x1 = d_a2-d_a3;
  }
  else if (d_a2*d_a3 > 0 || d_a1 != 0)
  {
    a = p_a1;
    b = (p_a2-p_a1)*d_a1;
    c = (p_a3-p_a1)*d_a1;

    x0 = d_a1-d_a2;
    x1 = d_a1-d_a3;
  }
  else if (d_a2 != 0)
  {
    a = p_a2;
    b = (p_a1-p_a2)*d_a2;
    c = (p_a3-p_a2)*d_a2;

    x0 = d_a2-d_a1;
    x1 = d_a2-d_a3;
  }
  else if (d_a3 != 0)
  {
    a = p_a3;
    b = (p_a1-p_a3)*d_a3;
    c = (p_a2-p_a3)*d_a3;

    x0 = d_a3-d_a1;
    x1 = d_a3-d_a2;
  }
  else
  {
    // triangles are coplanar
    coplanar = true;
  }

  if (!coplanar)
  {
    // compute interval for triangle b
    if (d_b1d_b2 > 0) //b1, b2 on same side of a's plane, b3 on the other side
    {
      d = p_b3;
      e = (p_b1-p_b3)*d_b3;
      f = (p_b2-p_b3)*d_b3;

      y0 = d_b3-d_b1;
      y1 = d_b3-d_b2;
    }
    else if (d_b1d_b3 > 0) //b1, b3 on same side of a's plane, b2 on the other side
    {
      d = p_b2;
      e=(p_b1-p_b2)*d_b2;
      f=(p_b3-p_b2)*d_b2;

      y0=d_b2-d_b1;
      y1=d_b2-d_b3;
    }
    else if (d_b2*d_b3 > 0 || d_b1 != 0)
    {
      d = p_b1;
      e = (p_b2-p_b1)*d_b1;
      f = (p_b3-p_b1)*d_b1;

      y0 = d_b1-d_b2;
      y1 = d_b1-d_b3;
    }
    else if (d_b2 != 0)
    {
      d = p_b2;
      e = (p_b1-p_b2)*d_b2;
      f = (p_b3-p_b2)*d_b2;

      y0 = d_b2-d_b1;
      y1 = d_b2-d_b3;
    }
    else if (d_b3 != 0)
    {
      d = p_b3;
      e = (p_b1-p_b3)*d_b3;
      f = (p_b2-p_b3)*d_b3;

      y0 = d_b3-d_b1;
      y1 = d_b3-d_b2;
    }
    else
    {
      coplanar = true;
    }
  }

  //special case when triangles are coplanar
  if (coplanar)
  {
    //check if they intersect in their common plane
    vgl_point_3d<double> i_pnt;
    if (vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p1,a_p2), b_p1, b_p2, b_p3, i_pnt) ||
        vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p2,a_p3), b_p1, b_p2, b_p3, i_pnt) ||
        vgl_triangle_3d_line_intersection(vgl_line_segment_3d<double>(a_p3,a_p1), b_p1, b_p2, b_p3, i_pnt) )
    {
      return Coplanar;
    }

    // finally, test if triangle a is totally contained in triangle b or vice versa
    if (vgl_triangle_3d_test_inside(a_p1, b_p1, b_p2, b_p3) ||
        vgl_triangle_3d_test_inside(b_p1, a_p1, a_p2, a_p3))
    {
      return Coplanar;
    }

    return None;
  }

  // finally, test if the triangles respective intervals
  // of the intersection line overlap
  xx = x0*x1;
  yy = y0*y1;
  xxyy = xx*yy;

  tmp = a*xxyy;
  isect1[0] = tmp+b*x1*yy;
  isect1[1] = tmp+c*x0*yy;

  tmp = d*xxyy;
  isect2[0] = tmp+e*xx*y1;
  isect2[1] = tmp+f*xx*y0;

  if (isect1[0] > isect1[1])
  {
    tmp = isect1[0];
    isect1[0] = isect1[1];
    isect1[1] = tmp;
  }

  if (isect2[0] > isect2[1])
  {
    tmp = isect2[0];
    isect2[0] = isect2[1];
    isect2[1] = tmp;
  }

  if (isect1[1] < isect2[0] || isect2[1] < isect1[0])
  {
    return None;
  }

  return Skew;
}

//=======================================================================
//: Compute the line of intersection of the given triangle and plane
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
//  \return intersection type
//  \note an intersection line is not defined (vgl_vgl_nan) for a coplanar intersection
vgl_triangle_3d_intersection_t vgl_triangle_3d_plane_intersection(
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3,
  const vgl_plane_3d<double>& i_plane,
  vgl_line_segment_3d<double>& i_line)
{
  //Firstly check if the triangle actually intersects the plane
  // by computing the signed distance of its vertices to the plane

  //all we care about is the sign of the distance
  double p1_d = i_plane.nx()*p1.x() + i_plane.ny()*p1.y() + i_plane.nz()*p1.z() + i_plane.d();
  double p2_d = i_plane.nx()*p2.x() + i_plane.ny()*p2.y() + i_plane.nz()*p2.z() + i_plane.d();
  double p3_d = i_plane.nx()*p3.x() + i_plane.ny()*p3.y() + i_plane.nz()*p3.z() + i_plane.d();

  // coplanarity robustness check
  if (std::fabs(p1_d) < sqrteps) p1_d = 0.0;
  if (std::fabs(p2_d) < sqrteps) p2_d = 0.0;
  if (std::fabs(p3_d) < sqrteps) p3_d = 0.0;

  vgl_line_3d_2_points<double> edge;

  if (p1_d*p2_d > 0 && p1_d*p3_d > 0) // all distances strictly same sign => no intersection
  {
    i_line.set(vgl_point_3d<double>(),vgl_point_3d<double>());
    return None;
  }
  else if (p1_d == 0 && p2_d == 0 && p3_d == 0) //triangle lies in plane
  {
    vgl_point_3d<double> i_pnt1; i_pnt1.set(vgl_nan, vgl_nan, vgl_nan);
    i_line.set(i_pnt1,i_pnt1);
    return Coplanar;
  }
  else if (p1_d*p2_d > 0) //p1, p2 on same side, p3 on the other
  {
    edge.set(p1,p3);
    vgl_point_3d<double> i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p2,p3);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(i_pnt1,i_pnt2);
  }
  else if (p1_d*p3_d > 0) //p1, p3 on same side, p2 on the other
  {
    edge.set(p1,p2);
    vgl_point_3d<double> i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p3,p2);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(i_pnt1,i_pnt2);
  }
  else if (p2_d*p3_d > 0) //p2, p3 on same side, p1 on the other
  {
    edge.set(p2,p1);
    vgl_point_3d<double> i_pnt1 = vgl_intersection(edge, i_plane);
    edge.set(p3,p1);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(i_pnt1,i_pnt2);
  }
  else if (p1_d == 0 && p2_d == 0) //edge p1,p2 in plane
  {
    i_line.set(p1,p2);
  }
  else if (p1_d == 0 && p3_d == 0) //edge p1,p3 in plane
  {
    i_line.set(p1,p3);
  }
  else if (p3_d == 0 && p2_d == 0) //edge p2,p3 in plane
  {
    i_line.set(p2,p3);
  }
  else if (p1_d == 0) //just p1 in plane
  {
    edge.set(p3,p2);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(p1,i_pnt2);
  }
  else if (p2_d == 0) //just p2 in plane
  {
    edge.set(p3,p1);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(p2,i_pnt2);
  }
  else if (p3_d == 0) //just p3 in plane
  {
    edge.set(p2,p1);
    vgl_point_3d<double> i_pnt2 = vgl_intersection(edge, i_plane);
    i_line.set(p3,i_pnt2);
  }
  return Skew;
}


//=======================================================================
// Compute the closest point on a triangle to a reference point
//=======================================================================
vgl_point_3d<double> vgl_triangle_3d_closest_point(
  const vgl_point_3d<double>& q,
  const vgl_point_3d<double>& p1,
  const vgl_point_3d<double>& p2,
  const vgl_point_3d<double>& p3)
{
  // Handle degenerate case.
  if (p1 == p2)
  {
    if (p2 == p3) return p3;
    return vgl_closest_point(vgl_line_3d_2_points<double>(p3, p1), q);
  }
  if (p2 == p3)
    return vgl_closest_point(vgl_line_3d_2_points<double>(p1, p2), q);
  if (p3 == p1)
    return vgl_closest_point(vgl_line_3d_2_points<double>(p2, p3), q);

  // Construct a plane from the 3 vertices of the triangle
  vgl_plane_3d<double> plane = create_plane_and_ignore_degenerate(p1, p2, p3);

  // Find the closest point on the whole plane to the test point
  vgl_point_3d<double> cp = vgl_closest_point<double>(plane, q);

  // Is this closest point inside the triangle ?
  if (vgl_triangle_3d_test_inside(cp, p1, p2, p3))
  {
    return cp;
  }
  else
  {
    // Find the nearest point on the triangle's boundary by testing each edge

    // Edge 1
    double cp1x, cp1y, cp1z;
    vgl_closest_point_to_linesegment(
      cp1x, cp1y, cp1z,
      p1.x(), p1.y(), p1.z(),
      p2.x(), p2.y(), p2.z(),
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp1(cp1x, cp1y, cp1z);
    double d1 = vgl_distance(cp1, q);

    // Edge 2
    double cp2x, cp2y, cp2z;
    vgl_closest_point_to_linesegment(
      cp2x, cp2y, cp2z,
      p2.x(), p2.y(), p2.z(),
      p3.x(), p3.y(), p3.z(),
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp2(cp2x, cp2y, cp2z);
    double d2 = vgl_distance(cp2, q);

    // Edge 3
    double cp3x, cp3y, cp3z;
    vgl_closest_point_to_linesegment(
      cp3x, cp3y, cp3z,
      p1.x(), p1.y(), p1.z(),
      p3.x(), p3.y(), p3.z(),
      q.x(), q.y(), q.z());
    vgl_point_3d<double> cp3(cp3x, cp3y, cp3z);
    double d3 = vgl_distance(cp3, q);

    // Identify nearest edge and return closest point on that edge.
    if (d1<=d2 && d1<=d3)
      return cp1;
    else if (d2<=d1 && d2<=d3)
      return cp2;
    else
      return cp3;
  }
}


//=======================================================================
// Compute the distance to the closest point on a triangle from a reference point.
//=======================================================================
double vgl_triangle_3d_distance(const vgl_point_3d<double>& q,
                                const vgl_point_3d<double>& p1,
                                const vgl_point_3d<double>& p2,
                                const vgl_point_3d<double>& p3)
{
  vgl_point_3d<double> c = vgl_triangle_3d_closest_point(q, p1, p2, p3);
  return vgl_distance(c, q);
}

//=======================================================================
//: Check if the two triangles are coplanar
//  The triangles are represented by their respective vertices \a a_p1, \a a_p2, \a a_p3
//  and \a b_p1, \a b_p2, \a b_p3
bool vgl_triangle_3d_triangle_coplanar(
                            const vgl_point_3d<double>& a_p1,
                            const vgl_point_3d<double>& a_p2,
                            const vgl_point_3d<double>& a_p3,
                            const vgl_point_3d<double>& b_p1,
                            const vgl_point_3d<double>& b_p2,
                            const vgl_point_3d<double>& b_p3)
{
  return coplanar(a_p1,b_p1,b_p2,b_p3)
      && coplanar(a_p2,b_p1,b_p2,b_p3)
      && coplanar(a_p3,b_p1,b_p2,b_p3);
}

//=======================================================================
//: Compute the area of a triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
double vgl_triangle_3d_area(const vgl_point_3d<double> &p0,
                            const vgl_point_3d<double> &p1,
                            const vgl_point_3d<double> &p2 )
{
  vgl_vector_3d<double> edge_vector0;
  edge_vector0 = p0 - p1;
  vgl_vector_3d<double> edge_vector1;
  edge_vector1 = p0 - p2;

  vgl_vector_3d<double> area_vector;
  area_vector = cross_product( edge_vector0, edge_vector1 );

  double area;
  area = area_vector.length();
  area /= 2;

  return area;
}

//=======================================================================
//: Compute the aspect ration of a triangle
//  The triangle is represented by its vertices \a p1, \a p2, \a p3
double vgl_triangle_3d_aspect_ratio(
  const vgl_point_3d<double> &p0,
  const vgl_point_3d<double> &p1,
  const vgl_point_3d<double> &p2 )
{
  return vgl_triangle_3d_longest_side( p0, p1, p2 ) / vgl_triangle_3d_shortest_side( p0, p1, p2 );
}
