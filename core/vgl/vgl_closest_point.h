// This is core/vgl/vgl_closest_point.h
#ifndef vgl_closest_point_h_
#define vgl_closest_point_h_
//:
// \file
// \brief Set of closest-point functions
// \author Peter Vanroose
//
// All these functions have two arguments which are geometry objects, and
// return either a pair of points (one from each object) which are the ones
// closest to each other, or just 1 point in case one of the objects is a point.
//
// See also vgl_distance if you only need the shortest distance between two
// geometric objects and not the actual closest points.
//
// \verbatim
//  Modifications
//    5 June 2003 Peter Vanroose created from bits and pieces in vgl_distance
//    5 June 2003 Brendan McCane added closest-point algo for 3D lines
//   11 June 2003 Peter Vanroose added closest-point on 3D line from point
//   14 Nov. 2003 Peter Vanroose made all functions templated
//   25 Sept 2004 Peter Vanroose added full 3D interface
// \endverbatim

#include <vgl/vgl_fwd.h> // forward declare various vgl classes
#include <vcl_utility.h> // for vcl_pair<T,U>

//: Closest point to \a (x,y) on the line segment \a (x1,y1)-(x2,y2)
template <class T>
void vgl_closest_point_to_linesegment(T& ret_x, T& ret_y,
                                      T x1, T y1,
                                      T x2, T y2,
                                      T x, T y);

//: Closest point to \a (x,y,z) on the line segment \a (x1,y1,z1)-(x2,y2,z2)
template <class T>
void vgl_closest_point_to_linesegment(T& ret_x, T& ret_y, T& ret_z,
                                      T x1, T y1, T z1,
                                      T x2, T y2, T z2,
                                      T x, T y, T z);

//: Closest point to \a (x,y) on open polygon \a (px[i],py[i])
//  Also returns the index of the polygon line segment where this point lies.
template <class T>
int vgl_closest_point_to_non_closed_polygon(T& ret_x, T& ret_y,
                                            T const px[], T const py[], unsigned int n,
                                            T x, T y);

//: Closest point to \a (x,y,z) on open polygon \a (px[i],py[i],pz[i])
//  Also returns the index of the polygon line segment where this point lies.
template <class T>
int vgl_closest_point_to_non_closed_polygon(T& ret_x, T& ret_y, T& ret_z,
                                            T const px[], T const py[], T const pz[], unsigned int n,
                                            T x, T y, T z);

//: Closest point to \a (x,y) on closed polygon \a (px[i],py[i])
//  Also returns the index of the polygon line segment where this point lies.
template <class T>
int vgl_closest_point_to_closed_polygon(T& ret_x, T& ret_y,
                                        T const px[], T const py[], unsigned int n,
                                        T x, T y);

//: Closest point to \a (x,y,z) on closed polygon \a (px[i],py[i],pz[i])
//  Also returns the index of the polygon line segment where this point lies.
template <class T>
int vgl_closest_point_to_closed_polygon(T& ret_x, T& ret_y, T& ret_z,
                                        T const px[], T const py[], T const pz[], unsigned int n,
                                        T x, T y, T z);

//: Return the point on the given line closest to the origin
// \relates vgl_line_2d
// \relates vgl_point_2d
template <class T>
vgl_point_2d<T> vgl_closest_point_origin(vgl_line_2d<T> const& l);

//: Return the point on the given line closest to the origin
// \relates vgl_homg_line_2d
// \relates vgl_homg_point_2d
template <class T>
vgl_homg_point_2d<T> vgl_closest_point_origin(vgl_homg_line_2d<T> const& l);

//: Return the point on the given plane closest to the origin
// \relates vgl_plane_3d
// \relates vgl_point_3d
template <class T>
vgl_point_3d<T> vgl_closest_point_origin(vgl_plane_3d<T> const& pl);

//: Return the point on the given plane closest to the origin
// \relates vgl_homg_plane_3d
// \relates vgl_homg_point_3d
template <class T>
vgl_homg_point_3d<T> vgl_closest_point_origin(vgl_homg_plane_3d<T> const& pl);

//: Return the point on the given line closest to the origin
// \relates vgl_line_3d_2_points
// \relates vgl_point_3d
template <class T>
vgl_point_3d<T> vgl_closest_point_origin(vgl_line_3d_2_points<T> const& l);

//: Return the point on the given line closest to the origin
// \relates vgl_homg_line_3d_2_points
// \relates vgl_homg_point_3d
template <class T>
vgl_homg_point_3d<T> vgl_closest_point_origin(vgl_homg_line_3d_2_points<T> const& l);

//: Return the point on the given line closest to the given point
// \relates vgl_point_2d
// \relates vgl_line_2d
template <class T>
vgl_point_2d<T> vgl_closest_point(vgl_line_2d<T> const& l,
                                  vgl_point_2d<T> const& p);
template <class T> inline
vgl_point_2d<T> vgl_closest_point(vgl_point_2d<T> const& p,
                                  vgl_line_2d<T> const& l)
{ return vgl_closest_point(l,p); }

//: Return the point on the given line closest to the given point
// \relates vgl_homg_point_2d
// \relates vgl_homg_line_2d
template <class T>
vgl_homg_point_2d<T> vgl_closest_point(vgl_homg_line_2d<T> const& l,
                                       vgl_homg_point_2d<T> const& p);
template <class T> inline
vgl_homg_point_2d<T> vgl_closest_point(vgl_homg_point_2d<T> const& p,
                                       vgl_homg_line_2d<T> const& l)
{ return vgl_closest_point(l,p); }

//: Return the point on the given plane closest to the given point
// \relates vgl_point_3d
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_closest_point(vgl_plane_3d<T> const& pl,
                                  vgl_point_3d<T> const& p);
template <class T> inline
vgl_point_3d<T> vgl_closest_point(vgl_point_3d<T> const& p,
                                  vgl_plane_3d<T> const& pl)
{ return vgl_closest_point(pl,p); }

//: Return the point on the given plane closest to the given point
// \relates vgl_homg_point_3d
// \relates vgl_homg_plane_3d
template <class T>
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_plane_3d<T> const& pl,
                                       vgl_homg_point_3d<T> const& p);
template <class T> inline
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_point_3d<T> const& p,
                                       vgl_homg_plane_3d<T> const& pl)
{ return vgl_closest_point(pl,p); }

//: Return the point on the given polygon closest to the given point
//  If the third argument is "false", the edge from last to first point of
//  each polygon sheet is not considered part of the polygon.
// \relates vgl_point_2d
// \relates vgl_polygon
template <class T>
vgl_point_2d<T> vgl_closest_point(vgl_polygon<T> const& poly,
                                  vgl_point_2d<T> const& point,
                                  bool closed=true);

template <class T> inline
vgl_point_2d<T> vgl_closest_point(vgl_point_2d<T> const& point,
                                  vgl_polygon<T> const& poly,
                                  bool closed=true)
{ return vgl_closest_point(poly, point, closed); }

//: Return the two points of nearest approach of two 3D lines, one on each line.
//
// There are 3 cases: the lines intersect (hence these two points are equal);
// the lines are parallel (an infinite number of solutions viz all points);
// the lines are neither parallel nor do they intersect (the general case).
// This method handles all 3 cases. In all cases, a pair of points is returned;
// in case 1, the two returned points are equal;
// in case 2, both points are the common point at infinity of the two lines.
//
// Note that case 2 also comprises the case where the given lines are identical.
// Hence, when observing a point at infinity as a return value, one should
// interpret this as "all points are closest points".
//
// \param line1
// \param line2
//
// \return vcl_pair<vgl_homg_point_3d<T>,vgl_homg_point_3d<T>>
//
// \author Paul Bourke, modified for use in VXL by Brendan McCane
//
// \notes This routine is adapted from code written by Paul Bourke and
// available online at
// http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/

template <class T>
vcl_pair<vgl_homg_point_3d<T>, vgl_homg_point_3d<T> >
vgl_closest_points(vgl_homg_line_3d_2_points<T> const& line1,
                   vgl_homg_line_3d_2_points<T> const& line2);

//: Return the point on the given line which is closest to the given point.
//  If the given point is at infinity, the point at infinity of the line is returned.
template <class T>
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_line_3d_2_points<T> const& l,
                                       vgl_homg_point_3d<T> const& p);

template <class T> inline
vgl_homg_point_3d<T> vgl_closest_point(vgl_homg_point_3d<T> const& p,
                                       vgl_homg_line_3d_2_points<T> const& l)
{ return vgl_closest_point(l,p); }

//: Return the point on the given line which is closest to the given point.
template <class T>
vgl_point_3d<T> vgl_closest_point(vgl_line_3d_2_points<T> const& l,
                                  vgl_point_3d<T> const& p);

template <class T> inline
vgl_point_3d<T> vgl_closest_point(vgl_point_3d<T> const& p,
                                  vgl_line_3d_2_points<T> const& l)
{ return vgl_closest_point(l,p); }

#endif // vgl_closest_point_h_
