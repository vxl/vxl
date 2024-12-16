// This is core/vgl/vgl_intersection.h
#ifndef vgl_intersection_h_
#define vgl_intersection_h_
//:
// \file
// \brief Set of intersection functions
// \author Jan 25, 2007 Gamze Tunali
//
// For intersections of "homogeneous coordinates" objects like vgl_homg_line_2d<T>,
// see the static methods of vgl/algo/vgl_homg_operators_2d<T> and _3d.
//
// \verbatim
//  Modifications
//   01 Mar 2007 - Gamze Tunali - split up into vgl/algo and vgl parts
//   21 Jul 2009 - Peter Vanroose - added box intersection (2d and 3d)
//   21 Jul 2009 - Peter Vanroose - added inlined point intersection functions
// \endverbatim

#include <vector>
#include "vgl_fwd.h"      // forward declare various vgl classes
#include "vgl_box_2d.h"   // method "contains()"
#include "vgl_box_3d.h"   // method "contains()"
#include "vgl_point_2d.h" // method "operator==()"
#include "vgl_point_3d.h" // method "operator==()"
#include "vgl_line_3d_2_points.h"
#include "vgl_line_segment_3d.h"
#include "vgl_infinite_line_3d.h"
#include "vgl_pointset_3d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return true if the two points intersect, i.e., coincide
// \relatesalso vgl_point_2d
template <class T>
inline bool
vgl_intersection(const vgl_point_2d<T> & p0, const vgl_point_2d<T> & p1)
{
  return p0 == p1;
}

//: Return true if the two points intersect, i.e., coincide
// \relatesalso vgl_point_2d
template <class T>
inline bool
vgl_intersection(const vgl_point_3d<T> & p0, const vgl_point_3d<T> & p1)
{
  return p0 == p1;
}

//: Return true if line intersects box. If so, compute intersection points.
// \relatesalso vgl_line_2d
template <class T>
bool
vgl_intersection(const vgl_box_2d<T> & box, const vgl_line_2d<T> & line, vgl_point_2d<T> & p0, vgl_point_2d<T> & p1);

//: Return true if line intersects box.If so,return the line segment inside box.
// \relatesalso vgl_line_2d
template <class T>
bool
vgl_intersection(const vgl_box_2d<T> & box, const vgl_line_segment_2d<T> & line, vgl_line_segment_2d<T> & int_line);

//: Returns the number of intersections of a line segment with a box, up to two are returned in p0 and p1.(warning! one
//: intersection could be either p0 or p1)
// \relatesalso vgl_line_segment_2d
template <class T>
unsigned
vgl_intersection(const vgl_box_2d<T> & box,
                 const vgl_line_segment_2d<T> & line,
                 vgl_point_2d<T> & p0,
                 vgl_point_2d<T> & p1);

//: Return true if two line segments intersect. If so,return the intersection point.
// \relatesalso vgl_line_segment_2d
template <class T>
bool
vgl_intersection(const vgl_line_segment_2d<T> & line1, const vgl_line_segment_2d<T> & line2, vgl_point_2d<T> & int_pt);

//: Return the intersection point of two concurrent lines
// \relatesalso vgl_line_3d_2_points
// Allows intersection points outside the line segments
// Throws an assertion if lines not concurrent
template <class T>
vgl_point_3d<T>
vgl_intersection(const vgl_line_3d_2_points<T> & l1, const vgl_line_3d_2_points<T> & l2);

//: Return the intersection point of segments of two concurrent lines. Returns false if the intersection point is not
//: inside both line segments
// \relatesalso vgl_line_segment_3d.
//
template <class T>
bool
vgl_intersection(const vgl_line_segment_3d<T> & l1, const vgl_line_segment_3d<T> & l2, vgl_point_3d<T> & i_pnt);

//: Return the intersection point of segments of a concurrent line and line segment pair. Returns false if the
//: intersection point is not inside both line segments
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_line_3d_2_points
template <class T>
bool
vgl_intersection(const vgl_line_3d_2_points<T> & l1, const vgl_line_segment_3d<T> & l2, vgl_point_3d<T> & i_pnt);

template <class T>
inline bool
vgl_intersection(const vgl_line_segment_3d<T> & l1, const vgl_line_3d_2_points<T> & l2, vgl_point_3d<T> & i_pnt)
{
  return vgl_intersection(l2, l1, i_pnt);
}

//: Return the intersection point of infinite lines, if concurrent.
// \relatesalso vgl_infinite_line_3d
template <class T>
bool
vgl_intersection(const vgl_infinite_line_3d<T> & l1, const vgl_infinite_line_3d<T> & l2, vgl_point_3d<T> & i_pnt);

//: Return the intersection point of rays. Returns false if rays are parallel or intersect outside of positive ray
//: domain
// \relatesalso vgl_ray_3d
template <class T>
bool
vgl_intersection(const vgl_ray_3d<T> & r1, const vgl_ray_3d<T> & r2, vgl_point_3d<T> & i_pnt);

//: Return the intersection point of two lines. Return false if lines are parallel
// \relatesalso vgl_line_2d
template <class T>
bool
vgl_intersection(const vgl_line_2d<T> & line0, const vgl_line_2d<T> & line1, vgl_point_2d<T> & intersection_point);


//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_3d_2_points
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T>
vgl_intersection(const vgl_line_3d_2_points<T> & line, const vgl_plane_3d<T> & plane);

//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_plane_3d
template <class T>
bool
vgl_intersection(const vgl_line_segment_3d<T> & line, const vgl_plane_3d<T> & plane, vgl_point_3d<T> & i_pt);


//: Return the intersection point of a line and a plane.
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_plane_3d
template <class T>
bool
vgl_intersection(const vgl_infinite_line_3d<T> & line, const vgl_plane_3d<T> & plane, vgl_point_3d<T> & i_pt);

//: Return the intersection point of a ray and a plane.
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_plane_3d
template <class T>
bool
vgl_intersection(const vgl_ray_3d<T> & ray, const vgl_plane_3d<T> & plane, vgl_point_3d<T> & i_pt);

//: Return the intersection line of two planes.
// Returns false if planes are effectively parallel
// \relatesalso vgl_line_segment_3d
// \relatesalso vgl_plane_3d
template <class T>
bool
vgl_intersection(const vgl_plane_3d<T> & plane0, const vgl_plane_3d<T> & plane1, vgl_line_segment_3d<T> & line)
{
  vgl_infinite_line_3d<T> inf_l;
  bool status = vgl_intersection(plane0, plane1, inf_l);
  if (status)
    line.set(inf_l.point_t(T(0)), inf_l.point_t(T(1)));
  return status;
}

template <class T>
bool
vgl_intersection(const vgl_plane_3d<T> & plane0, const vgl_plane_3d<T> & plane1, vgl_line_3d_2_points<T> & line)
{
  vgl_infinite_line_3d<T> inf_l;
  bool status = vgl_intersection(plane0, plane1, inf_l);
  if (status)
    line.set(inf_l.point_t(T(0)), inf_l.point_t(T(1)));
  return status;
}

template <class T>
bool
vgl_intersection(const vgl_plane_3d<T> & plane0, const vgl_plane_3d<T> & plane1, vgl_infinite_line_3d<T> & line);

//: Return the intersection point of three planes.
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T>
vgl_intersection(const vgl_plane_3d<T> & p1, const vgl_plane_3d<T> & p2, const vgl_plane_3d<T> & p3);

//: Return true if any point on [p1,p2] is within tol of [q1,q2]
//  Tests two line segments for intersection or near intersection
//  (within given tolerance).
// \author Dan jackson
// \relatesalso vgl_point_2d
template <class T>
bool
vgl_intersection(const vgl_point_2d<T> & p1,
                 const vgl_point_2d<T> & p2,
                 const vgl_point_2d<T> & q1,
                 const vgl_point_2d<T> & q2,
                 double tol = 1e-6);

//: Return true if the point lies inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
inline bool
vgl_intersection(const vgl_box_2d<T> & b, const vgl_point_2d<T> & p)
{
  return b.contains(p);
}

//: Return true if the point lies inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
inline bool
vgl_intersection(const vgl_point_2d<T> & p, const vgl_box_2d<T> & b)
{
  return b.contains(p);
}

//: Return true if the point lies inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
inline bool
vgl_intersection(const vgl_box_3d<T> & b, const vgl_point_3d<T> & p)
{
  return b.contains(p);
}

//: Return true if the point lies inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
inline bool
vgl_intersection(const vgl_point_3d<T> & p, const vgl_box_3d<T> & b)
{
  return b.contains(p);
}

//: Return true if line intersects box. If so, compute intersection points.
// \relatesalso vgl_infinite_line_3d
template <class T>
bool
vgl_intersection(const vgl_box_3d<T> & box,
                 const vgl_infinite_line_3d<T> & line,
                 vgl_point_3d<T> & p0,
                 vgl_point_3d<T> & p1);
//: Return true if ray intersects box. If so, compute intersection points.
// If ray origin is inside box then p0==p1
// \relatesalso vgl_ray_3d
template <class T>
bool
vgl_intersection(const vgl_box_3d<T> & box, const vgl_ray_3d<T> & ray, vgl_point_3d<T> & p0, vgl_point_3d<T> & p1);
//: Return true if a box and plane intersect in 3D
// \relatesalso vgl_plane_3d
// \relatesalso vgl_box_3d
template <class T>
bool
vgl_intersection(const vgl_box_3d<T> & b, const vgl_plane_3d<T> & plane);


//: Return the intersection of two boxes (which is itself either a box, or empty)
// \relatesalso vgl_box_2d
template <class T>
vgl_box_2d<T>
vgl_intersection(const vgl_box_2d<T> &, const vgl_box_2d<T> &);

//: Return the intersection of two boxes (which is itself either a box, or empty)
// \relatesalso vgl_box_3d
template <class T>
vgl_box_3d<T>
vgl_intersection(const vgl_box_3d<T> &, const vgl_box_3d<T> &);

//: Return true if the box and polygon regions intersect, regions include boundaries
// \relatesalso vgl_polygon
// \relatesalso vgl_box_2d
template <class T>
bool
vgl_intersection(const vgl_box_2d<T> & b, const vgl_polygon<T> & poly);

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
std::vector<vgl_point_2d<T>>
vgl_intersection(const vgl_box_2d<T> & b, const std::vector<vgl_point_2d<T>> & p);

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_2d
// \relatesalso vgl_box_2d
template <class T>
std::vector<vgl_point_2d<T>>
vgl_intersection(const std::vector<vgl_point_2d<T>> & p, const vgl_box_2d<T> & b);

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
std::vector<vgl_point_3d<T>>
vgl_intersection(const vgl_box_3d<T> & b, const std::vector<vgl_point_3d<T>> & p);

//: Return the points from the list that lie inside the box
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
std::vector<vgl_point_3d<T>>
vgl_intersection(const std::vector<vgl_point_3d<T>> & p, const vgl_box_3d<T> & b);

//: Find the intersections of a line with a polygon( can have multiple sheets)
// \relatesalso vgl_line_2d
// \relatesalso vgl_point_2d
template <class T>
std::vector<vgl_point_2d<T>>
vgl_intersection(const vgl_polygon<T> & poly, const vgl_line_2d<T> & line);
template <class T>
std::vector<vgl_point_2d<T>>
vgl_intersection(const vgl_line_2d<T> & line, const vgl_polygon<T> & poly)
{
  return vgl_intersection(poly, line);
}
// SEE vgl_clip.h to compute the intersection (as well as other boolean operations) of two vgl_polygons

//: return the intersection of a pointset with a plane, given a tolerance tol
//  the normal distance from the plane to the point is compared to the tolerance
//  the points within tolerance are projected along the normal direction onto the plane
template <class T>
vgl_pointset_3d<T>
vgl_intersection(const vgl_plane_3d<T> & plane, const vgl_pointset_3d<T> & ptset, T tol);

template <class T>
vgl_pointset_3d<T>
vgl_intersection(const vgl_pointset_3d<T> & ptset, const vgl_plane_3d<T> & plane, T tol)
{
  return vgl_intersection(plane, ptset, tol);
}

//: intersection of a box with the pointset
template <class T>
vgl_pointset_3d<T>
vgl_intersection(const vgl_box_3d<T> & box, const vgl_pointset_3d<T> & ptset);

template <class T>
vgl_pointset_3d<T>
vgl_intersection(const vgl_pointset_3d<T> & ptset, const vgl_box_3d<T> & box)
{
  return vgl_intersection(box, ptset);
}


template <class T>
vgl_pointset_3d<T>
vgl_intersection(const vgl_plane_3d<T> & plane, const vgl_pointset_3d<T> & ptset, T tol);

#define VGL_INTERSECTION_INSTANTIATE(T) extern "please include vgl/vgl_intersection.hxx first"
#define VGL_INTERSECTION_BOX_INSTANTIATE(T) extern "please include vgl/vgl_intersection.hxx first"

#endif // vgl_intersection_h_
