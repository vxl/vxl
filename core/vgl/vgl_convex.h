// This is core/vgl/vgl_convex.h
#ifndef vgl_convex_h_
#define vgl_convex_h_
//:
// \file
// \brief Functions such as convex hull, convex union, convexify polygon, ...
// \author Peter Vanroose
// \date 14 November 2003

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>

//: Return a single-sheet polygon which is the smallest one containing all given points
template <class T> vgl_polygon<T> vgl_convex_hull(vcl_vector<vgl_point_2d<T> > const& points);

#endif // vgl_convex_h_
