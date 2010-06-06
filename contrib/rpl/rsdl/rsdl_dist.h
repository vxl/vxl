#ifndef rsdl_dist_h_
#define rsdl_dist_h_
//:
// \file
// \brief Point to point and point to bounding box distance functions.
// \author Chuck Stewart
// \date June 2001
//
//  Distance functions for rsdl points and bounding boxes.  These
//  points and bounding boxes have a mixture of cartesian and angular
//  coordinates, so the computation is a little more complicated than
//  ordinary Euclidean distance calculations.  Angles are treated in
//  a full 2*pi range, so that the maximum distance between two angles
//  is pi.  Also, bounding intervals on angles can be such that the
//  numerical value on the "min" angle is greater than the numerical
//  value on the "max" angle.  These are values that wrap-around 0.
//
//  Two assumptions are important.  First, it is assumed that all
//  angles are truly within a 2*pi range; it doesn't matter which.
//  This is an implementation (and efficiency) convenience.  Second,
//  any normalization for comparing angle and cartesian distance
//  measures has already been done.


#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_bounding_box.h>

//: Return the square distance between two rsdl_point's.
double
rsdl_dist_sq( const rsdl_point & p, const rsdl_point& q );

//: Return the distance between two rsdl_point's.
double
rsdl_dist( const rsdl_point & p, const rsdl_point& q );

//: Return the minimum square distance between \a p and any point in \a b.
double
rsdl_dist_sq( const rsdl_point & p, const rsdl_bounding_box& b );

//: Return the minimum distance between \a p and any point in \a b.
double
rsdl_dist( const rsdl_point & p, const rsdl_bounding_box& b );

//: Determine if a point is inside a bounding box.
bool
rsdl_dist_point_in_box( const rsdl_point & pt,
                        const rsdl_bounding_box & box );


//: Determine the relation between a purported "inner" box and a purported "outer" box.
//  \a inside will be true iff all points in \a inner are in \a outer
//  \a intersect will be true iff at least one point from \a inner is in \a outer.
void
rsdl_dist_box_relation( const rsdl_bounding_box & inner,
                        const rsdl_bounding_box & outer,
                        bool& inside,
                        bool& intersects );

#endif
