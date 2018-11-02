// This is core/vgl/algo/vgl_compute_rigid_3d.hxx
#ifndef vgl_compute_rigid_3d_hxx_
#define vgl_compute_rigid_3d_hxx_
//:
// \file

#include <iostream>
#include "vgl_compute_rigid_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/vnl_double_4.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/vnl_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

template <class T>
vgl_compute_rigid_3d<T>::
vgl_compute_rigid_3d(std::vector<vgl_point_3d<T> > const& points1,
                     std::vector<vgl_point_3d<T> > const& points2)
: points1_(points1),
  points2_(points2)
{
  assert(points1.size() == points2.size());
}

template <class T>
void vgl_compute_rigid_3d<T>::
add_points(vgl_point_3d<T> const &p1, vgl_point_3d<T> const &p2)
{
  points1_.push_back(p1);
  points2_.push_back(p2);
}


template <class T>
void vgl_compute_rigid_3d<T>::clear()
{
  points1_.clear();
  points2_.clear();
}


//: center all the points at the origin, and return the applied translation
template <class T>
void vgl_compute_rigid_3d<T>::
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
void vgl_compute_rigid_3d<T>::
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
bool vgl_compute_rigid_3d<T>::estimate()
{
  vgl_vector_3d<T> t1, t2;
  std::vector<vgl_point_3d<T> > pts1(points1_), pts2(points2_);
  center_points(pts1, t1);
  center_points(pts2, t2);

  T s1, s2;
  scale_points(pts2, s2);
  s1 = s2;
  for (unsigned i=0; i<pts1.size(); ++i)
  {
    vgl_point_3d<T>& p = pts1[i];
    p.set(p.x()*s1, p.y()*s1, p.z()*s1);
  }

  // estimate rotation
  vnl_matrix<T> M(3,3,0.0);
  for (unsigned i=0; i<pts1.size(); ++i)
  {
    vgl_point_3d<T>& p1 = pts1[i];
    vgl_point_3d<T>& p2 = pts2[i];
    M(0,0) += p1.x()*p2.x();
    M(0,1) += p1.x()*p2.y();
    M(0,2) += p1.x()*p2.z();
    M(1,0) += p1.y()*p2.x();
    M(1,1) += p1.y()*p2.y();
    M(1,2) += p1.y()*p2.z();
    M(2,0) += p1.z()*p2.x();
    M(2,1) += p1.z()*p2.y();
    M(2,2) += p1.z()*p2.z();
  }

  vnl_matrix<T> N(4,4);
  N(0,0) =   M(0,0) - M(1,1) - M(2,2);
  N(1,1) = - M(0,0) + M(1,1) - M(2,2);
  N(2,2) = - M(0,0) - M(1,1) + M(2,2);
  N(3,3) =   M(0,0) + M(1,1) + M(2,2);
  N(0,1) = N(1,0) = M(0,1) + M(1,0);
  N(0,2) = N(2,0) = M(2,0) + M(0,2);
  N(1,2) = N(2,1) = M(1,2) + M(2,1);
  N(3,0) = N(0,3) = M(1,2) - M(2,1);
  N(3,1) = N(1,3) = M(2,0) - M(0,2);
  N(3,2) = N(2,3) = M(0,1) - M(1,0);

  vnl_svd<T> svd(N);
  vnl_double_4 q(svd.V().get_column(0));
  rotation_ = vgl_rotation_3d<T>(vnl_quaternion<T>(q));
  translation_ = (rotation_*t1) - t2;

  return true;
}

//--------------------------------------------------------------------------
#undef VGL_COMPUTE_RIGID_3D_INSTANTIATE
#define VGL_COMPUTE_RIGID_3D_INSTANTIATE(T) \
template class vgl_compute_rigid_3d<T >

#endif // vgl_compute_rigid_3d_hxx_
