// This is vxl/vgl/vgl_distance.h
#ifndef vgl_distance_h_
#define vgl_distance_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Set of distance functions
// \author fsm@robots.ox.ac.uk
//
// Note that these functions return double, not the template parameter Type,
// since e.g. the distance between two vgl_point_2d<int> is not always an int.
//
// \verbatim
// Modifications:
//  2 July 2001 Peter Vanroose  added vgl_distance(point,line) and (point,plane)
//  2 July 2001 Peter Vanroose  inlined 4 functions and made return types double
// \endverbatim

#include <vgl/vgl_fwd.h> // forward declare various vgl classes

//: Squared distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
double vgl_distance2_to_linesegment(double x1, double y1,
                                    double x2, double y2,
                                    double x, double y);

//: Distance between point \a (x,y) and closest point on line segment \a (x1,y1)-(x2,y2)
double vgl_distance_to_linesegment(double x1, double y1,
                                   double x2, double y2,
                                   double x, double y);

//: Distance between point \a (x,y) and closest point on open polygon \a (px[i],py[i])
double vgl_distance_to_non_closed_polygon(float const px[], float const py[], unsigned n,
                                          double x, double y);

//: Distance between point \a (x,y) and closest point on closed polygon \a (px[i],py[i])
double vgl_distance_to_closed_polygon(float const px[], float const py[], unsigned n,
                                      double x, double y);

//: find the shortest distance of the line to the origin
// \relates vgl_line_2d
template <class Type>
double vgl_distance_origin(vgl_line_2d<Type> const& l);

//: find the shortest distance of the line to the origin
// \relates vgl_homg_line_2d
template <class Type>
double vgl_distance_origin(vgl_homg_line_2d<Type> const& l);

//: return the distance between two points
// \relates vgl_point_2d
template <class Type>
inline double vgl_distance(vgl_point_2d<Type>const& p1,
                           vgl_point_2d<Type>const& p2) {return length(p2-p1);}

//: return the distance between two points
// \relates vgl_point_3d
template <class Type>
inline double vgl_distance(vgl_point_3d<Type>const& p1,
                           vgl_point_3d<Type>const& p2) {return length(p2-p1);}

//: return the distance between two points
// \relates vgl_homg_point_2d
template <class Type>
inline double vgl_distance(vgl_homg_point_2d<Type>const& p1,
                           vgl_homg_point_2d<Type>const& p2) {
  return vgl_distance(vgl_point_2d<Type>(p1),vgl_point_2d<Type>(p2)); }

//: return the distance between two points
// \relates vgl_homg_point_3d
template <class Type>
inline double vgl_distance(vgl_homg_point_3d<Type>const& p1,
                           vgl_homg_point_3d<Type>const& p2) {
  return vgl_distance(vgl_point_3d<Type>(p1),vgl_point_3d<Type>(p2)); }

//: return the perpendicular distance from a point to a line in 2D
// \relates vgl_point_2d
// \relates vgl_line_2d
template <class Type>
double vgl_distance(vgl_line_2d<Type> const& l, vgl_point_2d<Type> const& p);
template <class Type>
inline double vgl_distance(vgl_point_2d<Type> const& p,
                           vgl_line_2d<Type> const& l) {return vgl_distance(l,p);}
template <class Type>
double vgl_distance(vgl_homg_line_2d<Type> const& l, vgl_homg_point_2d<Type> const& p);
//: return the perpendicular distance from a point to a line in 2D
// \relates vgl_homg_point_2d
// \relates vgl_homg_line_2d
template <class Type>
inline double vgl_distance(vgl_homg_point_2d<Type> const& p,
                           vgl_homg_line_2d<Type> const& l) {return vgl_distance(l,p);}

//: return the perpendicular distance from a point to a plane in 3D
// \relates vgl_point_3d
// \relates vgl_plane_3d
template <class Type>
double vgl_distance(vgl_plane_3d<Type> const& l, vgl_point_3d<Type> const& p);
template <class Type>
inline double vgl_distance(vgl_point_3d<Type> const& p,
                           vgl_plane_3d<Type> const& l) {return vgl_distance(l,p);}
//: return the perpendicular distance from a point to a plane in 3D
// \relates vgl_homg_point_3d
// \relates vgl_homg_plane_3d
template <class Type>
double vgl_distance(vgl_homg_plane_3d<Type> const& l, vgl_homg_point_3d<Type> const& p);
template <class Type>
inline double vgl_distance(vgl_homg_point_3d<Type> const& p,
                           vgl_homg_plane_3d<Type> const& l) {return vgl_distance(l,p);}

//: distance between a point and the closest point on the polygon.
// \relates vgl_point_2d
// \relates vgl_polygon
template <class Type>
double vgl_distance(vgl_polygon const& poly, vgl_point_2d<Type> const& point);

template <class Type>
inline double vgl_distance(vgl_point_2d<Type> const& point, vgl_polygon const& poly) {
  return vgl_distance(poly, point);
}

#endif // vgl_distance_h_
