// This is brl/bbas/bvgl/bvgl_volume_of_intersection.hxx
#ifndef bvgl_volume_of_intersection_hxx_
#define bvgl_volume_of_intersection_hxx_
//:
// \file
// \author Andrew Miller

#include <limits>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include "bvgl_volume_of_intersection.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_sphere_3d.h>

//tolerance constants/functions
static double bvgl_eps = 1.0e-8; // tolerance for intersections
inline bool bvgl_near_zero(double x) { return x < bvgl_eps && x > -bvgl_eps; }
inline bool bvgl_near_eq(double x, double y) { return bvgl_near_zero(x-y); }

//: calculate the volume of intersection between these two spheres
template <typename T>
T bvgl_volume_of_intersection(vgl_sphere_3d<T> const& A, vgl_sphere_3d<T> const& B)
{
  //distance between the two spheres
  T d = (A.centre() - B.centre()).length();
  T r0 = A.radius();
  T r1 = B.radius();

  //cases
  //0. if one sphere is completely inside the other one
  T difR = std::fabs(r0 - r1);
  if ( d <= difR )
  {
    //return the volume of the smaller sphere
    T minR = std::min(r0, r1);
    return (4.0/3.0) * vnl_math::pi * minR * minR * minR;
  }

  //2. The two spheres intersect...
  T sumR = r0 + r1;
  if (d > difR && d < sumR)
  {
    //calculate distance to the plane of intersection from the center of circle A
    T xInt = ( r0*r0 - r1*r1 + d*d ) / (2.0*d) ;

    //calculate height of sperhical caps
    T h0 = r0 - xInt;
    T h1 = r1 + xInt - d;

    // volume is sum of tops
    return (vnl_math::pi/3.0)*( h0*h0 * (3*r0-h0) + h1*h1 * (3*r1-h1) );
  }

  //otherwise the two spheres do not intersect
  return (T) 0.0;
}


//---------------------------------------------------------------------------
//Template instantiation
#undef BVGL_VOLUME_OF_INTERSECTION_INSTANTIATE
#define BVGL_VOLUME_OF_INTERSECTION_INSTANTIATE(T) \
template T bvgl_volume_of_intersection(vgl_sphere_3d<T > const& A, vgl_sphere_3d<T > const& B)

#endif // bvgl_volume_of_intersection_hxx_
