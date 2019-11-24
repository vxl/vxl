// This is core/vgl/vgl_distance.h
#ifndef vgl_distance_h_
#define vgl_distance_h_
//:
// \file
// \brief Set of distance functions
// \author fsm
//
// Note that these functions return double, not the template parameter Type,
// since e.g. the distance between two vgl_point_2d<int> is not always an int,
// but even the squared distance between such a point and a line is not integer.
//
// \verbatim
//  Modifications
//    2 July 2001 Peter Vanroose added vgl_distance(point,line) and (point,plane)
//    2 July 2001 Peter Vanroose inlined 4 functions and made return types double
//    2 Jan. 2003 Peter Vanroose corrected functions returning negative distance
//    5 June 2003 Peter Vanroose added vgl_distance(line_3d,line_3d)
//   11 June 2003 Peter Vanroose added vgl_distance(line_3d,point_3d)
//   14 Nov. 2003 Peter Vanroose made all functions templated
//   25 Sept 2004 Peter Vanroose added 3D vgl_distance_to_linesegment()
//   25 Sept 2004 Peter Vanroose added 3D vgl_distance_to_*_polygon()
// \endverbatim

#include "vgl_fwd.h" // forward declare various vgl classes

//: Squared distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
template <class T>
double vgl_distance2_to_linesegment(T x1, T y1,
                                    T x2, T y2,
                                    T x, T y);

//: Distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
template <class T>
double vgl_distance_to_linesegment(T x1, T y1,
                                   T x2, T y2,
                                   T x, T y);

//: Squared distance between point \a (x,y,z) and closest point on line segment \a (x1,y1,z1)-(x2,y2,z2)
template <class T>
double vgl_distance2_to_linesegment(T x1, T y1, T z1,
                                    T x2, T y2, T z2,
                                    T x,  T y,  T z);

//: Distance between point \a (x,y,z) and closest point on line segment \a (x1,y1,z1)-(x2,y2,z2)
template <class T>
double vgl_distance_to_linesegment(T x1, T y1, T z1,
                                   T x2, T y2, T z2,
                                   T x,  T y,  T z);

//: Distance between point \a (x,y) and closest point on open polygon \a (px[i],py[i])
template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], unsigned int n,
                                          T x, T y);

//: Distance between point \a (x,y,z) and closest point on open polygon \a (px[i],py[i],pz[i])
template <class T>
double vgl_distance_to_non_closed_polygon(T const px[], T const py[], T const pz[], unsigned int n,
                                          T x, T y, T z);

//: Distance between point \a (x,y) and closest point on closed polygon \a (px[i],py[i])
template <class T>
double vgl_distance_to_closed_polygon(T const px[], T const py[], unsigned int n,
                                      T x, T y);

//: Distance between point \a (x,y,z) and closest point on closed polygon \a (px[i],py[i]),pz[i]
template <class T>
double vgl_distance_to_closed_polygon(T const px[], T const py[], T const pz[], unsigned int n,
                                      T x, T y, T z);

//: find the shortest distance of the line to the origin
// \relatesalso vgl_line_2d
template <class T>
double vgl_distance_origin(vgl_line_2d<T> const& l);

//: find the shortest distance of the plane to the origin
// \relatesalso vgl_plane_3d
template <class T>
double vgl_distance_origin(vgl_plane_3d<T> const& pl);

//: find the shortest distance of the line to the origin
// \relatesalso vgl_line_3d_2_points
template <class T>
double vgl_distance_origin(vgl_line_3d_2_points<T> const& l);

//: find the shortest distance of the line to the origin
// \relatesalso vgl_homg_line_2d
template <class T>
double vgl_distance_origin(vgl_homg_line_2d<T> const& l);

//: find the shortest distance of the plane to the origin
// \relatesalso vgl_homg_plane_3d
template <class T>
double vgl_distance_origin(vgl_homg_plane_3d<T> const& pl);

//: find the shortest distance of the line to the origin
// \relatesalso vgl_homg_line_3d_2_points
template <class T>
double vgl_distance_origin(vgl_homg_line_3d_2_points<T> const& l);

//: return the distance between two points
// \relatesalso vgl_point_2d
template <class T> inline
double vgl_distance(vgl_point_2d<T>const& p1,
                    vgl_point_2d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relatesalso vgl_point_3d
template <class T> inline
double vgl_distance(vgl_point_3d<T>const& p1,
                    vgl_point_3d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relatesalso vgl_homg_point_1d
template <class T>
double vgl_distance(vgl_homg_point_1d<T>const& p1,
                    vgl_homg_point_1d<T>const& p2);

//: return the distance between two points
// \relatesalso vgl_homg_point_2d
template <class T> inline
double vgl_distance(vgl_homg_point_2d<T>const& p1,
                    vgl_homg_point_2d<T>const& p2) { return length(p2-p1); }

//: return the distance between two points
// \relatesalso vgl_homg_point_3d
template <class T> inline
double vgl_distance(vgl_homg_point_3d<T>const& p1,
                    vgl_homg_point_3d<T>const& p2) { return length(p2-p1); }

//: return the perpendicular distance from a point to a line in 2D
// \relatesalso vgl_point_2d
// \relatesalso vgl_line_2d
template <class T>
double vgl_distance(vgl_line_2d<T> const& l,
                    vgl_point_2d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_2d<T> const& p,
                    vgl_line_2d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a line in 2D
// \relatesalso vgl_homg_point_2d
// \relatesalso vgl_homg_line_2d
template <class T>
double vgl_distance(vgl_homg_line_2d<T> const& l,
                    vgl_homg_point_2d<T> const& p);
template <class T> inline
double vgl_distance(vgl_homg_point_2d<T> const& p,
                    vgl_homg_line_2d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a plane in 3D
// \relatesalso vgl_point_3d
// \relatesalso vgl_plane_3d
template <class T>
double vgl_distance(vgl_plane_3d<T> const& l,
                    vgl_point_3d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_plane_3d<T> const& l) { return vgl_distance(l,p); }

//: return the perpendicular distance from a point to a plane in 3D
// \relatesalso vgl_homg_point_3d
// \relatesalso vgl_homg_plane_3d
template <class T>
double vgl_distance(vgl_homg_plane_3d<T> const& l,
                    vgl_homg_point_3d<T> const& p);
template <class T> inline
double vgl_distance(vgl_homg_point_3d<T> const& p,
                    vgl_homg_plane_3d<T> const& l) { return vgl_distance(l,p); }

template <class T>
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_sphere_3d<T> const& s);
//: distance between a point and the closest point on the polygon.
//  If the third argument is "false", the edge from last to first point of
//  each polygon sheet is not considered part of the polygon.
// \relatesalso vgl_point_2d
// \relatesalso vgl_polygon
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
// \relatesalso vgl_homg_line_3d_2_points

template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& line1,
                    vgl_homg_line_3d_2_points<T> const& line2);

//: Return the perpendicular distance from a point to a line in 3D.
//  See vgl_closest_point.h for more information.
// \relatesalso vgl_homg_line_3d_2_points
template <class T>
double vgl_distance(vgl_homg_line_3d_2_points<T> const& l,
                    vgl_homg_point_3d<T> const& p);

template <class T> inline
double vgl_distance(vgl_homg_point_3d<T> const& p,
                    vgl_homg_line_3d_2_points<T> const& l) { return vgl_distance(l,p); }

//: Return the perpendicular distance from a point to a line in 3D.
//  See vgl_closest_point.h for more information.
// \relatesalso vgl_line_3d_2_points
template <class T>
double vgl_distance(vgl_line_3d_2_points<T> const& l,
                    vgl_point_3d<T> const& p);

template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_line_3d_2_points<T> const& l) { return vgl_distance(l,p); }

//: return the closest distance from a point to a ray
template <class T>
double vgl_distance(vgl_ray_3d<T> const& r,
                    vgl_point_3d<T> const& p);

template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_ray_3d<T> const& r) { return vgl_distance(r,p); }

//: return the closest distance from a point to an infinite line
template <class T>
double vgl_distance(vgl_infinite_line_3d<T> const& l,
                    vgl_point_3d<T> const& p);

template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_infinite_line_3d<T> const& l) { return vgl_distance(l,p); }
//: Closest distance from a point \a p to a line segment \a l in 2D
// \relatesalso vgl_point_2d
// \relatesalso vgl_line_segment_2d
// \sa vgl_distance_to_linesegment()
// \sa vgl_distance2_to_linesegment()
template <class T>
double vgl_distance(vgl_line_segment_2d<T> const& l,
                    vgl_point_2d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_2d<T> const& p,
                    vgl_line_segment_2d<T> const& l) { return vgl_distance(l,p); }


//: Closest distance from a point \a p to a line segment \a l in 3D
// \relatesalso vgl_point_3d
// \relatesalso vgl_line_segment_3d
// \sa vgl_distance_to_linesegment()
// \sa vgl_distance2_to_linesegment()
template <class T>
double vgl_distance(vgl_line_segment_3d<T> const& l,
                    vgl_point_3d<T> const& p);
template <class T> inline
double vgl_distance(vgl_point_3d<T> const& p,
                    vgl_line_segment_3d<T> const& l) { return vgl_distance(l,p); }

//: closest distance from a point to a box (2d)
template <class T>
double vgl_distance(vgl_point_2d<T> const& p, vgl_box_2d<T> const& b);
template <class T>
double vgl_distance(vgl_box_2d<T> const& b, vgl_point_2d<T> const& p){return vgl_distance(p,b);}


#endif // vgl_distance_h_
