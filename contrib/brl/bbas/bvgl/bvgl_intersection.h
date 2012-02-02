// This is brl/bbas/bvgl/bvgl_intersection.h
#ifndef bvgl_intersection_h_
#define bvgl_intersection_h_
//:
// \file
// \brief Set of intersection functions
// \author Jan 30, 2012 Andrew Miller
//

#include <vgl/vgl_fwd.h> // forward declare various vgl classes
#include <vcl_vector.h>
#include <vgl/vgl_sphere_3d.h>
#include <vgl/vgl_box_3d.h>
#include "bvgl_triangle_3d.h"

//: returns true if 3d box A intersects with 3d triangle B
template <typename T>
bool bvgl_intersection(vgl_box_3d<T> const& A, bvgl_triangle_3d<T> const& B);

#define BVGL_INTERSECTION(T) extern "please include bbas/bvgl/bvgl_intersection.txx first"

#endif // bvgl_intersection_h_
