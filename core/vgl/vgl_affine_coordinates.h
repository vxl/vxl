// This is core/vgl/vgl_affine_coordinates.h
#ifndef vgl_affine_coordinates_h_
#define vgl_affine_coordinates_h_
//:
// \file
// \brief Computes 2-d and 3-d affine coordinates of point sets
//
// \author    J.L. Mundy
//            Jan 30, 2017
//
// \verbatim
//  Modifications
//  None
// \endverbatim
//
//-------------------------------------------------------------------------------
// Converts points into baycentric coordinates given an affine basis
// For 2-d, three points are required to form the basis
// For 3-d, four points are required to form the basis
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>

// Points are all coplanar. The first three points in pts are the basis, pts[0] is the origin
template <class T>
void vgl_affine_coordinates_2d(std::vector<vgl_point_2d<T> > const& pts, std::vector<vgl_point_2d<T> >& affine_pts);

// The first four points in pts are the basis.
template <class T>
void vgl_affine_coordinates_3d(std::vector<vgl_point_3d<T> > const& pts, std::vector<vgl_point_3d<T> >& affine_pts);

// Two 2-d pointsets define the 3-d basis(V0,V1,V2). The first four points in pts1 and pts2 are points that define
// this basis, i.e. V0 = pts[1]-pts[0], V1 = pts[2]-pts[0], etc..
// Based on the paper Affine Structure From Motion, J.Koenderink and A. van Doorn, J. Optical Society Am. Vol8,No.2, 1991
// The 2-d pointsets can be regarded as points from affine camera projections of a 3-d pointset.
// The reconstruction of a point P pts[i], i>3, is based on the idea that V2 and P project along the
// line of sight to the same points V2' and P' on the plane of V0 x V1 in view 1.
// In a different view, view2, V2 and V2', and P and P' will be displaced along parallel lines (the line of sight of view1).
// V2' and P' will project to the same 2-d affine  coordinates in view 2.
// The ratio of these parallel displacement distances is used to define the 3-d affine coordintes of P.
template <class T>
void vgl_affine_coordinates_3d(std::vector<vgl_point_2d<T> > const& pts1, std::vector<vgl_point_2d<T> > const& pts2,
                               std::vector<vgl_point_3d<T> >& affine_pts);
#define VGL_AFFINE_COORDINATES_INSTANTIATE(T) extern "please include vgl/vgl_affine_coordinates.hxx first"
#endif // vgl_affine_coordinates_h_
