#ifndef vgl_distance_h_
#define vgl_distance_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_distance
// .INCLUDE vgl/vgl_distance.h
// .FILE vgl_distance.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
// .SECTION Modifications
//   Peter Vanroose - 1 December 2000 - added vgl_distance_origin(line_2d)
//                                      and vgl_distance(point,point)
//

// squared distance
double vgl_distance2_to_linesegment(double x1, double y1,
                                    double x2, double y2,
                                    double x, double y);

double vgl_distance_to_linesegment(double x1, double y1,
                                   double x2, double y2,
                                   double x, double y);

double vgl_distance_to_non_closed_polygon(float const px[], float const py[], unsigned n,
                                          double x, double y);

double vgl_distance_to_closed_polygon(float const px[], float const py[], unsigned n,
                                      double x, double y);

#include "vgl_line_2d.h"
#include "vgl_homg_line_2d.h"

//: find the shortest distance of the line to the origin
// NOTE: currently only to be instantiated for double or float!
template <class Type>
Type vgl_distance_origin(vgl_line_2d<Type> const& l);
template <class Type>
Type vgl_distance_origin(vgl_homg_line_2d<Type> const& l);

#include "vgl_point_2d.h"
#include "vgl_point_3d.h"
#include "vgl_homg_point_2d.h"
#include "vgl_homg_point_3d.h"

//: return the distance between two points
// NOTE: currently only to be instantiated for double or float!
template <class Type>
Type vgl_distance(vgl_point_2d<Type>const& p1, vgl_point_2d<Type>const& p2);
template <class Type>
Type vgl_distance(vgl_point_3d<Type>const& p1, vgl_point_3d<Type>const& p2);
template <class Type>
Type vgl_distance(vgl_homg_point_2d<Type>const& p1, vgl_homg_point_2d<Type>const& p2);
template <class Type>
Type vgl_distance(vgl_homg_point_3d<Type>const& p1, vgl_homg_point_3d<Type>const& p2);

#endif // vgl_distance_h_
