// This is core/vgl/algo/vgl_intersection.h
#ifndef vgl_intersection_h_
#define vgl_intersection_h_
//:
// \file
// \brief Set of intersection functions
// \author Jan 25, 2007 Gamze Tunali
//
// This file aims to gather all the intersection methods on vgl classes
// at one place. Some of the functions moved from their own class
// files to here or interface methods created for non-homogeneous versions
// of the ones that are already defined in vgl/algo/vgl_homg_operators_3d
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgl/vgl_fwd.h> // forward declare various vgl classes
#include <vcl_vector.h>

//: Return box defining intersection between boxes a and b.
//  Empty box returned if no intersection.
// \relates vgl_box_3d
template <class T>
vgl_box_3d<T> intersect(vgl_box_3d<T> const& a,
                        vgl_box_3d<T> const& b);

//: Return the intersection point of two concurrent lines
// \relates vgl_line_3d_2_points
template <class T>
vgl_point_3d<T> intersection(vgl_line_3d_2_points<T> const& l1,
                             vgl_line_3d_2_points<T> const& l2);

//: Return the intersection point of a line and a plane.
// \relates vgl_line_3d_2_points
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> intersection(vgl_line_3d_2_points<T> const& line,
                             vgl_plane_3d<T> const& plane);

//: Return the intersection point of three planes.
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> intersection(const vgl_plane_3d<T>& p1,
                             const vgl_plane_3d<T>& p2,
                             const vgl_plane_3d<T>& p3);

//: Return the intersection point of vector of planes.
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> intersection(const vcl_vector<vgl_plane_3d<T> >& p);

#define VGL_INTERSECTION_INSTANTIATE(T) extern "please include vgl/vgl_intersection.txx first"

#endif // vgl_intersection_h_
