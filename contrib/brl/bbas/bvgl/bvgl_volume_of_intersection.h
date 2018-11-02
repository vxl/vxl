// This is brl/bbas/bvgl/bvgl_volume_of_intersection.h
#ifndef bvgl_volume_of_intersection_h_
#define bvgl_volume_of_intersection_h_
//:
// \file
// \brief Set of intersection functions
// \author Jan 25, 2007 Gamze Tunali
//
// For intersections of "homogeneous coordinates" objects like vgl_homg_line_2d<T>,
// see the static methods of vgl/algo/vgl_homg_operators_2d<T> and _3d.
//
// \verbatim
//  Modifications
//   01 Mar 2007 - Gamze Tunali - split up into vgl/algo and vgl parts
//   21 Jul 2009 - Peter Vanroose - added box intersection (2d and 3d)
//   21 Jul 2009 - Peter Vanroose - added inlined point intersection functions
// \endverbatim

#include <iostream>
#include <vector>
#include <vgl/vgl_fwd.h> // forward declare various vgl classes
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_sphere_3d.h>

template <typename T>
T bvgl_volume_of_intersection(vgl_sphere_3d<T> const& A, vgl_sphere_3d<T> const& B);

#define BVGL_VOLUME_OF_INTERSECTION(T) extern "please include bbas/bvgl/bvgl_volume_of_intersection.txx first"

#endif // bvgl_volume_of_intersection_h_
