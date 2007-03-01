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

//: Return the intersection point of vector of planes.
// \relates vgl_plane_3d
template <class T>
vgl_point_3d<T> vgl_intersection(const vcl_vector<vgl_plane_3d<T> >& p);

#define VGL_INTERSECTION_INSTANTIATE(T) extern "please include vgl/algo/vgl_intersection.txx first"

#endif // vgl_intersection_h_
