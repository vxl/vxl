// This is core/vgl/vgl_distance.h
#ifndef vgl_distance_h_
#define vgl_distance_h_
//:
// \file
// \brief Set of distance functions
// \author fsm
//
// Note that these functions return double, not the template parameter Type,
// since e.g. the distance between two vgl_point_2d<int> is not always an int.
//
// \verbatim
//  Modifications
//    2 July 2001 Peter Vanroose added vgl_distance(point,line) and (point,plane)
//    2 July 2001 Peter Vanroose inlined 4 functions and made return types double
//    2 Jan. 2003 Peter Vanroose corrected functions returning negative distance
//    5 June 2003 Peter Vanroose added vgl_distance(line_3d,line_3d)
//   11 June 2003 Peter Vanroose added vgl_distance(line_3d,point_3d)
//   14 Nov. 2003 Peter Vanroose made all functions templated
// \endverbatim

#include <vgl/vgl_fwd.h> // forward declare various vgl classes

//: Squared distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
template <class T>
T vgl_distance2_to_linesegment(T x1, T y1,
                               T x2, T y2,
                               T x, T y);

//: Distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
template <class T>
double vgl_distance_to_linesegment(T x1, T y1,
                                   T x2, T y2,
                                   T x, T y);

//: Distance between point \a (x,y) and closest point on open polygon \a (px[i],py[i])
template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], unsigned int n,
                                          T x, T y);

//: Distance between point \a (x,y) and closest point on closed polygon \a (px[i],py[i])
template <class T>
double vgl_distance_to_closed_polygon(T const px[], T const py[], unsigned int n,
                                      T x, T y);

//: find the shortest distance of the line to the origin
// \relates vgl_line_2d
template <class T>
double vgl_distance_origin(vgl_line_2d<T> const& l);

//: find the shortest distance of the line to the origin
// \relates vgl_homg_line_2d
template <class T>
double vgl_distance_origin(vgl_homg_line_2d<T> const& l);

//: return the distance between two points
// \relates vgl_point_2d
template <class T> inline
double vgl_distance(vgl_point_2d<T>const& p1,
                    vgl_point_2d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relates vgl_point_3d
template <class T> inline
double vgl_distance(vgl_point_3d<T>const& p1,
                    vgl_point_3d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relates vgl_homg_point_1d
template <class T>
double vgl_distance(vgl_homg_point_1d<T>const& p1,
                    vgl_homg_point_1d<T>const& p2);

//: return the distance between two points
// \relates vgl_homg_point_2d
template <class T> inline
double vgl_distance(vgl_homg_point_2d<T>const& p1,
                    vgl_homg_point_2d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relates vgl_homg_point_3d
template <class T> inline
double vgl_distance(vgl_homg_point_3d<T>const& p1,
                    vgl_homg_point_3d<T>const& p2) { return length(p2-p1); }

//: return the perpendicular distance from a point to a line in 2D
// \relates vgl_point_2d
// \relates vgl_line_2d
template <class T>
double vgl_distance(vgl_line_2d<T> const& l,
                    vgl_point_2d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_2d<T> const& p,
                    vgl_line_2d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a line in 2D
// \relates vgl_homg_point_2d
// \relates vgl_homg_line_2d
template <class T>
double vgl_distance(vgl_homg_line_2d<T> const& l,
                    vgl_homg_point_2d<T> const& p);
template <class T> inline
double vgl_distance(vgl_homg_point_2d<T> const& p,
                    vgl_homg_line_2d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a plane in 3D
// \relates vgl_point_3d
// \relates vgl_plane_3d
template <class T>
double vgl_distance(vgl_plane_3d<T> const& l,
                    vgl_point_3d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_plane_3d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a plane in 3D
// \relates vgl_homg_point_3d
// \relates vgl_homg_plane_3d
template <class T>
double vgl_distance(vgl_homg_plane_3d<T> const& l,
                    vgl_homg_point_3d<T> const& p);
template <class T> inline
double vgl_distance(vgl_homg_point_3d<T> const& p,
                    vgl_homg_plane_3d<T> const& l) { return vgl_distance(l,p); }

//: distance between a point and the closest point on the polygon.
//  If the third argument is "false", the edge from last to first point of
//  each polygon sheet is not considered part of the polygon.
// \relates vgl_point_2d
// \relates vgl_polygon
template <class T>
double vgl_distance(vgl_polygon<T> const& poly,
                    vgl_point_2d<T> const& point,
                    bool closed=true);

template <class T> inline
double vgl_distance(vgl_point_2d<T> const& point,
                    vgl_polygon<T> const& poly,
                    bool closed=true) { return vgl_distance(poly,point,closed); }

//: Return the perpendicular distance between two lines in 3D.
//  See vgl_closest_point.h for more information.

template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& line1,
                    vgl_homg_line_3d_2_points<T> const& line2);

template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& l,
                    vgl_homg_point_3d<T> const& p);

template <class T> inline
double vgl_distance(vgl_homg_point_3d<T> const& p,
                    vgl_homg_line_3d_2_points<T> const& l) { return vgl_distance(l,p); }

#endif // vgl_distance_h_
