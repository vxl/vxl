// This is core/vgl/algo/vgl_fit_plane_3d.hxx
#ifndef vgl_fit_plane_3d_hxx_
#define vgl_fit_plane_3d_hxx_
//:
// \file

#include <iostream>
#include <utility>
#include "vgl_fit_plane_3d.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matrix.h>

template <class T>
vgl_fit_plane_3d<T>::vgl_fit_plane_3d(std::vector<vgl_homg_point_3d<T> > points)
: points_(std::move(points))
{
}

template <class T>
void vgl_fit_plane_3d<T>::add_point(vgl_homg_point_3d<T> const &p)
{
  points_.push_back(p);
}

template <class T>
void vgl_fit_plane_3d<T>::add_point(const T x, const T y, const T z)
{
  points_.push_back(vgl_homg_point_3d<T> (x, y, z));
}

 template <class T>
 void vgl_fit_plane_3d<T>::clear()
{
  points_.clear();
}

 template <class T>
 bool vgl_fit_plane_3d<T>::fit(const T error_marg, std::ostream* errstream/*=0*/)
{

  T min = this->fit(errstream);
  if (min > error_marg) {
    if (errstream) *errstream << "Error Margin " << error_marg << '<' << min << ". Could not fit the points to a plane\n";
    return false;
  }

  return true;
}

template <class T>
T vgl_fit_plane_3d<T>::fit(std::ostream* errstream)
{
    // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream) {
    *errstream << "there is a problem with norm transform\n";
  }

  // compute the matrix A of Ax=b
  T A=0, B=0, C=0, D=0, E=0, F=0, G=0, H=0, I=0;
  const unsigned n = static_cast<const unsigned>(points_.size());
  for (unsigned i=0; i<n; i++) {
    points_[i] = norm(points_[i]);//normalize
    const T x = points_[i].x()/points_[i].w();
    const T y = points_[i].y()/points_[i].w();
    const T z = points_[i].z()/points_[i].w();
    A += x;
    B += y;
    C += z;
    D += x*x;
    E += y*y;
    F += z*z;
    G += x*y;
    H += y*z;
    I += x*z;
  }

  vnl_matrix<T> coeff_matrix(4,4);
  coeff_matrix(0, 0) = D;
  coeff_matrix(0, 1) = G;
  coeff_matrix(0, 2) = I;
  coeff_matrix(0, 3) = A;

  coeff_matrix(1, 0) = G;
  coeff_matrix(1, 1) = E;
  coeff_matrix(1, 2) = H;
  coeff_matrix(1, 3) = B;

  coeff_matrix(2, 0) = I;
  coeff_matrix(2, 1) = H;
  coeff_matrix(2, 2) = F;
  coeff_matrix(2, 3) = C;

  coeff_matrix(3, 0) = A;
  coeff_matrix(3, 1) = B;
  coeff_matrix(3, 2) = C;
  coeff_matrix(3, 3) = T(n);

  vnl_svd<T> svd(coeff_matrix);
  // check if the error_margin is achieved

  // null vector gives the solution to the linear equation where b=[0]
  vnl_vector<T> s = svd.nullvector();

  // re-transform the points back to the real world
  vnl_matrix_fixed<T,4,4> N_transp=norm.get_matrix().transpose();
  s = N_transp * s;

  T a, b, c, d;
  a = s.get(0);
  b = s.get(1);
  c = s.get(2);
  d = s.get(3);
  plane_ = vgl_homg_plane_3d<T> (a, b, c, d);

  return svd.sigma_min();
}

//--------------------------------------------------------------------------
#undef VGL_FIT_PLANE_3D_INSTANTIATE
#define VGL_FIT_PLANE_3D_INSTANTIATE(T) \
template class vgl_fit_plane_3d<T >

#endif // vgl_fit_plane_3d_hxx_
