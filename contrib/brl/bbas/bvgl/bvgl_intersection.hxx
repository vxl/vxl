// This is brl/bbas/bvgl/bvgl_intersection.hxx
#ifndef bvgl_intersection_hxx_
#define bvgl_intersection_hxx_
//:
// \file
// \author Andrew Miller
//
//  implementation based on code from Tomas Akenine-Moller
//  http://jgt.akpeters.com/papers/AkenineMoller01/tribox.html
//
//  \modifications
//   11 June 2015 dec: incorporated bug fixes included in v3, available at: http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
//

#include <limits>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include "bvgl_intersection.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//helper functions
namespace bvgl_intersection_helpers
{
  //label axes (for iterating)
  enum Axes { X=0, Y=1, Z=2 };

  template <typename T>
  inline void cross(T dest[3], T a[3], T b[3]) {
    dest[0]=a[1]*b[2]-a[2]*b[1];
    dest[1]=a[2]*b[0]-a[0]*b[2];
    dest[2]=a[0]*b[1]-a[1]*b[0];
  }

  template <typename T>
  inline T dot(T a[3], T b[3]) { return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }

  template <typename T>
  inline void subtract(T dest[3], T a[3], T b[3]) {
    dest[0] = a[0]-b[0];
    dest[1] = a[1]-b[1];
    dest[2] = a[2]-b[2];
  }

  template <typename T>
  inline void findMinMax(T x0, T x1, T x2, T& min, T&max) {
    min = max = x0;
    if (x1 < min) min = x1;
    if (x1 > max) max = x1;
    if (x2 < min) min = x2;
    if (x2 > max) max = x2;
  }

  template <typename T>
  bool planeBoxIntersect(T normal[3], T vert[3], T maxbox[3]) {
    T vmin[3],vmax[3],v;
    for (int q=X;q<=Z;++q)
      {
      v = vert[q];
      if (normal[q]>0.0f)
        {
        vmin[q]=-maxbox[q] - v;
        vmax[q]=maxbox[q] - v;
        }
      else
        {
        vmin[q]=maxbox[q] - v;
        vmax[q]=-maxbox[q] - v;
        }
      }
    return ( (dot(normal,vmin) <= 0.0f) && (dot(normal,vmax) >= 0.0f) );
  }

  template<int Axis0, int Axis1, typename T>
  inline bool axisTest(T edge[3], T fedge[3], T v0[3], T v1[3], T boxhalfsize[3]) {
    T p0 = edge[Axis1]*v0[Axis0] - edge[Axis0]*v0[Axis1];
    T p1 = edge[Axis1]*v1[Axis0] - edge[Axis0]*v1[Axis1];
    T min = std::min(p0, p1);
    T max = std::max(p0, p1);
    T rad = fedge[Axis1]*boxhalfsize[Axis0] + fedge[Axis0]*boxhalfsize[Axis1];
    return min <= rad && max >= -rad;
  }
}


//: calculate the volume of intersection between these two spheres
template <typename T>
bool bvgl_intersection(vgl_box_3d<T> const& A, bvgl_triangle_3d<T> const& B)
{
  // use separating axis theorem to intersect triangle and box */
  using namespace bvgl_intersection_helpers;

  //check to see if the triangle slices the box
  T boxcenter[3] = { A.centroid_x(), A.centroid_y(), A.centroid_z() };
  T boxhalfsize[3] = { A.width()/2.0, A.height()/2.0, A.depth()/2.0 };
  T triverts[3][3] = { { B[0].x(), B[0].y(), B[0].z() },
                       { B[1].x(), B[1].y(), B[1].z() },
                       { B[2].x(), B[2].y(), B[2].z() } };

  //translate box to origin (tri verts subtracted)
  T v0[3],v1[3],v2[3];
  subtract(v0,triverts[0],boxcenter);
  subtract(v1,triverts[1],boxcenter);
  subtract(v2,triverts[2],boxcenter);

  // compute triangle edges
  T e0[3],e1[3],e2[3];
  subtract(e0,v1,v0);
  subtract(e1,v2,v1);
  subtract(e2,v0,v2);

  //Test 1: test AABB collision (Box to tri AABB)
  T min, max;
  findMinMax(v0[X],v1[X],v2[X],min,max);
  if (min>boxhalfsize[X] || max<-boxhalfsize[X])
    return false;
  findMinMax(v0[Y],v1[Y],v2[Y],min,max);
  if (min>boxhalfsize[Y] || max<-boxhalfsize[Y])
    return false;
  findMinMax(v0[Z],v1[Z],v2[Z],min,max);
  if (min>boxhalfsize[Z] || max<-boxhalfsize[Z])
    return false;

  //Test 2: test box/plane intersection
  T normal[3];
  cross(normal,e0,e1);
  if (!planeBoxIntersect(normal,v0,boxhalfsize))
    return false;

  //test 3: if plane/box do intersect, test bounds
  T fe[3] = { std::fabs(e0[X]), std::fabs(e0[Y]), std::fabs(e0[Z]) };
  if (!axisTest<Y,Z>(e0, fe, v0, v2, boxhalfsize) ||
      !axisTest<X,Z>(e0, fe, v0, v2, boxhalfsize) ||
      !axisTest<X,Y>(e0, fe, v1, v2, boxhalfsize) )
    return false;

  fe[X]=std::fabs(e1[X]); fe[Y]=std::fabs(e1[Y]); fe[Z]=std::fabs(e1[Z]);
  if (!axisTest<Y,Z>(e1, fe, v0, v2, boxhalfsize) ||
      !axisTest<X,Z>(e1, fe, v0, v2, boxhalfsize) ||
      !axisTest<X,Y>(e1, fe, v0, v1, boxhalfsize) )
    return false;

  fe[X]=std::fabs(e2[X]); fe[Y]=std::fabs(e2[Y]); fe[Z]=std::fabs(e2[Z]);
  if (!axisTest<Y,Z>(e2, fe, v0, v1, boxhalfsize) ||
      !axisTest<X,Z>(e2, fe, v0, v1, boxhalfsize) ||
      !axisTest<X,Y>(e2, fe, v1, v2, boxhalfsize) )
    return false;

  //otherwise box and tri overlap
  return true;
}


//---------------------------------------------------------------------------
//Template instantiation
#undef BVGL_INTERSECTION_INSTANTIATE
#define BVGL_INTERSECTION_INSTANTIATE(T) \
template bool bvgl_intersection(vgl_box_3d<T > const& A, bvgl_triangle_3d<T > const& B)

#endif // bvgl_intersection_hxx_
