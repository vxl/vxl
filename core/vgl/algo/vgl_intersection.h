// This is core/vgl/algo/vgl_intersection.h
#ifndef vgl_algo_intersection_h_
#define vgl_algo_intersection_h_
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
//   23 Jul 2009 - Gamze Tunali - added a 3D box-polygon intersection method
//
//   01 Mar 2007 - Gamze Tunali - split up into vgl/algo and vgl parts
// \endverbatim

#include <vector>
#include <list>
#include <vgl/vgl_fwd.h> // forward declare various vgl classes
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return the intersection point of vector of planes.
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(const std::vector<vgl_plane_3d<T> >& p);

//: Return the intersection line of a set of planes, use list to distinguish from point return
// \relatesalso vgl_plane_3d
// \relatesalso vgl_infinite_line_3d
template <class T>
vgl_infinite_line_3d<T>
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes);

//: Return the intersection line of a set of weighted planes, use list to distinguish from point return
// \relatesalso vgl_plane_3d
// \relatesalso vgl_infinite_line_3d

template <class T>
bool
vgl_intersection(const std::list<vgl_plane_3d<T> >& planes, std::vector<T> ws, vgl_infinite_line_3d<T>& line, T & residual);

//: Return true if the box and polygon intersect in 3-d, regions include boundaries
// Polygon is represented as an ordered vector of 3-d points
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
bool vgl_intersection(vgl_box_3d<T> const& b, std::list<vgl_point_3d<T> >& p);


#define VGL_ALGO_INTERSECTION_INSTANTIATE(T) extern "please include vgl/algo/vgl_intersection.hxx first"

#endif // vgl_algo_intersection_h_
