// This is core/vgl/algo/vgl_compute_similarity_3d.hxx
#ifndef vgl_compute_similarity_3d_hxx_
#define vgl_compute_similarity_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_compute_similarity_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/vnl_double_4.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include "vgl_compute_rigid_3d.h"

template <class T>
vgl_compute_similarity_3d<T>::
vgl_compute_similarity_3d(std::vector<vgl_point_3d<T> > const& points1,
                          std::vector<vgl_point_3d<T> > const& points2)
: points1_(points1),
  points2_(points2)
{
  assert(points1.size() == points2.size());
}

template <class T>
void vgl_compute_similarity_3d<T>::
add_points(vgl_point_3d<T> const &p1, vgl_point_3d<T> const &p2)
{
  points1_.push_back(p1);
  points2_.push_back(p2);
}


template <class T>
void vgl_compute_similarity_3d<T>::clear()
{
  points1_.clear();
  points2_.clear();
}


//: center all the points at the origin, and return the applied translation
template <class T>
void vgl_compute_similarity_3d<T>::
center_points(std::vector<vgl_point_3d<T> >& pts,
              vgl_vector_3d<T>& t) const
{
  t.set(0,0,0);
  vgl_point_3d<T> origin(0,0,0);
  for (unsigned i=0; i<pts.size(); ++i)
  {
    t += origin - pts[i];
  }
  t /= pts.size();
  for (unsigned i=0; i<pts.size(); ++i)
  {
    pts[i] += t;
  }
}


//: normalize the scale of the points, and return the applied scale
//  The average distance from the origin will be sqrt(3)
template <class T>
void vgl_compute_similarity_3d<T>::
scale_points(std::vector<vgl_point_3d<T> >& pts,
             T& s) const
{
  s = 0.0;
  vgl_point_3d<T> origin(0,0,0);
  for (unsigned i=0; i<pts.size(); ++i)
  {
    s += (pts[i]-origin).length();
  }
  s = std::sqrt(3.0)*pts.size()/s;
  for (unsigned i=0; i<pts.size(); ++i)
  {
    vgl_point_3d<T>& p = pts[i];
    p.set(p.x()*s, p.y()*s, p.z()*s);
  }
}


template <class T>
bool vgl_compute_similarity_3d<T>::estimate()
{
  vgl_vector_3d<T> t1, t2;
  std::vector<vgl_point_3d<T> > pts1(points1_), pts2(points2_);
  center_points(pts1, t1);
  center_points(pts2, t2);

  T s1, s2;
  scale_points(pts2, s2);
  scale_points(pts1, s1);
  scale_ = s1/s2;

  vgl_compute_rigid_3d<T> rigid_comp(pts1, pts2);
  rigid_comp.estimate();

  rotation_ = rigid_comp.rotation();
  translation_ = (scale_*(rotation_*t1) - t2);

  return true;
}

//--------------------------------------------------------------------------
#undef VGL_COMPUTE_SIMILARITY_3D_INSTANTIATE
#define VGL_COMPUTE_SIMILARITY_3D_INSTANTIATE(T) \
template class vgl_compute_similarity_3d<T >

#endif // vgl_compute_similarity_3d_hxx_
