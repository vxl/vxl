// This is core/vgl/algo/vgl_intersection.txx
#ifndef vgl_algo_intersection_txx_
#define vgl_algo_intersection_txx_
//:
// \file
// \author Gamze Tunali

#include "vgl_intersection.h"
#include <vcl_vector.h>
// not used? #include <vcl_limits.h>

#include <vgl/algo/vgl_homg_operators_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>


template <class T>
vgl_point_3d<T> vgl_intersection(const vcl_vector<vgl_plane_3d<T> >& p)
{
  vcl_vector<vgl_homg_plane_3d<T> > planes;
  for (unsigned i=0; i<p.size(); i++) {
    planes.push_back(vgl_homg_plane_3d<T> (p[i]));
  }

  return vgl_homg_operators_3d<T>::intersection(planes);
}

#undef VGL_ALGO_INTERSECTION_INSTANTIATE
#define VGL_ALGO_INTERSECTION_INSTANTIATE(T) \
template vgl_point_3d<T > vgl_intersection(const vcl_vector<vgl_plane_3d<T > >&)

#endif // vgl_algo_intersection_txx_
