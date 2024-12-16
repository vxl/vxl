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
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return the intersection point of vector of planes.
// \relatesalso vgl_plane_3d
template <class T>
vgl_point_3d<T>
vgl_intersection(const std::vector<vgl_plane_3d<T>> & p);

//: Return the intersection line of a set of planes, use list to distinguish from point return
// \relatesalso vgl_plane_3d
// \relatesalso vgl_infinite_line_3d
template <class T>
vgl_infinite_line_3d<T>
vgl_intersection(const std::list<vgl_plane_3d<T>> & planes);

//: Return the intersection line of a set of weighted planes, use list to distinguish from point return
// \relatesalso vgl_plane_3d
// \relatesalso vgl_infinite_line_3d

template <class T>
bool
vgl_intersection(const std::list<vgl_plane_3d<T>> & planes,
                 std::vector<T> ws,
                 vgl_infinite_line_3d<T> & line,
                 T & residual);

//: Return true if the box and polygon intersect in 3-d, regions include boundaries
// Polygon is represented as an ordered vector of 3-d points
// \relatesalso vgl_point_3d
// \relatesalso vgl_box_3d
template <class T>
bool
vgl_intersection(const vgl_box_3d<T> & b, std::list<vgl_point_3d<T>> & p);

//: Return true if a set of rays produce a well-conditioned solution for their intersection
// The intersection algorithm finds the 3D point that produces the smallest sum of squared
// orthogonal distances to each ray.
template <class T>
bool
vgl_intersection(const std::vector<vgl_ray_3d<T>> & rays, vgl_point_3d<T> & inter_pt);

//: Return true if a set of rays produce a well-conditioned solution for their intersection
//  Weight each ray according to a covariance matrix the 2-d orthogonal ray displacement vector,
//  since a displacement along a ray does not affect the intersection point. It follows that for n rays,
//  the covariance matrix is a 2n x 2n matrix. The coordinate system for the 2-d displacement vector
//  is defined by the vgl_plane_3d<T>::plane_coord_vectors(vgl_vector_3d<T>& uvec, vgl_vector_3d<T>& vvec)
//  method.
//
template <class T>
bool
vgl_intersection(const std::vector<vgl_ray_3d<T>> & rays, const vnl_matrix<T> & covar, vgl_point_3d<T> & inter_pt);

// special case of two rays
// also returns the closest distance between the rays
template <class T>
bool
vgl_intersection(const vgl_ray_3d<T> & ray0, const vgl_ray_3d<T> & ray1, vgl_point_3d<T> & inter_pt, T & dist);

#define VGL_ALGO_INTERSECTION_INSTANTIATE(T) extern "please include vgl/algo/vgl_intersection.hxx first"

#endif // vgl_algo_intersection_h_
