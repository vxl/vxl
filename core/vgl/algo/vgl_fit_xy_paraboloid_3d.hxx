// This is core/vgl/algo/vgl_fit_xy_paraboloid_3d.hxx
#ifndef vgl_fit_xy_paraboloid_3d_hxx_
#define vgl_fit_xy_paraboloid_3d_hxx_
//:
// \file

#include <iostream>
#include <limits>
#include "vgl_fit_xy_paraboloid_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>

template <class T>
vgl_fit_xy_paraboloid_3d<T>::vgl_fit_xy_paraboloid_3d(std::vector<vgl_point_3d<T>> points)

{
  for (typename std::vector<vgl_point_3d<T>>::iterator pit = points.begin(); pit != points.end(); ++pit)
    points_.push_back(vgl_homg_point_3d<T>(*pit));
  p_ = vnl_matrix<T>(6, 1, T(0));
}

template <class T>
void
vgl_fit_xy_paraboloid_3d<T>::add_point(const vgl_point_3d<T> & p)
{
  points_.push_back(vgl_homg_point_3d<T>(p));
}

template <class T>
void
vgl_fit_xy_paraboloid_3d<T>::add_point(const T x, const T y, const T z)
{
  points_.push_back(vgl_homg_point_3d<T>(x, y, z));
}

template <class T>
void
vgl_fit_xy_paraboloid_3d<T>::clear()
{
  points_.clear();
}

template <class T>
T
vgl_fit_xy_paraboloid_3d<T>::fit_linear(std::ostream * errstream)
{
  const size_t n = points_.size();
  if (!n)
  {
    if (errstream)
      *errstream << "No points to fit xy_paraboloid\n";
    return T(-1);
  }
  // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream)
  {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }
  vnl_matrix<double> M(6, 6, 0.0);
  vnl_matrix<double> p, b(6, 1, 0.0);
  for (size_t i = 0; i < n; i++)
  {
    vgl_homg_point_3d<T> hp = norm(points_[i]); // normalize
    double x = static_cast<double>(hp.x()), y = static_cast<double>(hp.y()), z = static_cast<double>(hp.z()),
           w = static_cast<double>(hp.w());
    M[0][0] += x * x * x * x;
    M[0][1] += x * x * x * y;
    M[0][2] += x * x * y * y;
    M[0][3] += x * x * x;
    M[0][4] += x * x * y;
    M[0][5] += x * x;
    M[1][0] += x * x * x * y;
    M[1][1] += x * x * y * y;
    M[1][2] += x * y * y * y;
    M[1][3] += x * x * y;
    M[1][4] += x * y * y;
    M[1][5] += x * y;
    M[2][0] += x * x * y * y;
    M[2][1] += x * y * y * y;
    M[2][2] += y * y * y * y;
    M[2][3] += x * y * y;
    M[2][4] += y * y * y;
    M[2][5] += y * y;
    M[3][0] += x * x * x;
    M[3][1] += x * x * y;
    M[3][2] += x * y * y;
    M[3][3] += x * x;
    M[3][4] += x * y;
    M[3][5] += x;
    M[4][0] += x * x * y;
    M[4][1] += x * y * y;
    M[4][2] += y * y * y;
    M[4][3] += x * y;
    M[4][4] += y * y;
    M[4][5] += y;
    M[5][0] += x * x;
    M[5][1] += x * y;
    M[5][2] += y * y;
    M[5][3] += x;
    M[5][4] += y;
    M[5][5] += 1.0;
    b[0][0] += z * x * x;
    b[1][0] += z * x * y;
    b[2][0] += z * y * y;
    b[3][0] += z * x;
    b[4][0] += z * y;
    b[5][0] += z;
  }
  M /= n;
  b /= n;
  vnl_svd<double> svd(M);
  size_t rank = svd.rank();
  if (rank < 6)
  {
    if (errstream)
    {
      *errstream << "Insufficient rank " << rank << " to solve for paraboloid" << std::endl;
    }
    return T(-1);
  }
  p = svd.solve(b);
  // un-normalize the p vector
  vnl_matrix_fixed<T, 4, 4> N = norm.get_matrix();
  T s = N[0][0], tx = N[0][3], ty = N[1][3], tz = N[2][3];
  p_[0][0] = p[0][0] * s;
  p_[1][0] = p[1][0] * s;
  p_[2][0] = p[2][0] * s;
  p_[3][0] = (p[3][0] + p[1][0] * ty + 2.0 * p[0][0] * tx);
  p_[4][0] = (p[4][0] + p[1][0] * tx + 2.0 * p[2][0] * ty);
  p_[5][0] =
    ((p[5][0] - tz) + p[0][0] * tx * tx + p[1][0] * tx * ty + p[2][0] * ty * ty + p[3][0] * tx + p[4][0] * ty) / s;

  // Transform the quadric back to the original coordinate frame
  //  z = f(x,y) = p0x^2 + p1xy + p2y^2 + p3x + p4y + p5
  // full q = ax^2 + by^2 + cz^2 + dxy + exz + fyz + gxw +hyw +izw +jw^2 = 0
  // a = p0, b = p2, c = 0, d = p1, e = 0, f = 0, g = p3, h = p4, i = -1, j = p5
  paraboloid_linear_.set(p_[0][0], p_[2][0], 0.0, p_[1][0], 0.0, 0.0, p_[3][0], p_[4][0], -1.0, p_[5][0]);

  // compute average z error for the pointset
  T z_err_sum = 0.0;
  for (unsigned i = 0; i < n; i++)
  {
    T zerr = fabs(z(points_[i].x(), points_[i].y()) - points_[i].z());
    z_err_sum += zerr;
  }
  z_err_sum /= n;

  return static_cast<T>(z_err_sum);
}

template <class T>
std::vector<vgl_point_3d<T>>
vgl_fit_xy_paraboloid_3d<T>::points() const
{
  std::vector<vgl_point_3d<T>> ret;
  const size_t n = points_.size();
  for (size_t i = 0; i < n; i++)
  {
    vgl_point_3d<T> p(points_[i]);
    ret.push_back(p);
  }
  return ret;
}
template <class T>
T
vgl_fit_xy_paraboloid_3d<T>::z(T x, T y) const
{
  vnl_matrix<T> q(6, 1);
  q[0][0] = x * x;
  q[1][0] = x * y;
  q[2][0] = y * y;
  q[3][0] = x;
  q[4][0] = y;
  q[5][0] = 1;
  T v = 0;
  for (size_t i = 0; i < 6; ++i)
    v += q[i][0] * p_[i][0]; // dot product
  return v;
}
template <class T>
vgl_point_2d<T>
vgl_fit_xy_paraboloid_3d<T>::extremum_point() const
{

  double det = 4.0 * p_[0][0] * p_[2][0] - p_[1][0] * p_[1][0];
  if (fabs(det) < 1.0e-8)
  {
    std::cout << "singular extremum determinant returning NAN" << std::endl;
    return vgl_point_2d<T>(NAN, NAN);
  }
  vnl_matrix_fixed<double, 2, 2> pinv;
  pinv[0][0] = 2.0 * p_[2][0];
  pinv[0][1] = -p_[1][0];
  pinv[1][0] = -p_[1][0];
  pinv[1][1] = 2.0 * p_[0][0];
  pinv /= det;
  vnl_matrix_fixed<double, 2, 1> b, xy;
  b[0][0] = -p_[3][0];
  b[1][0] = -p_[4][0];
  xy = pinv * b;
  return vgl_point_2d<T>(xy[0][0], xy[1][0]);
}
//--------------------------------------------------------------------------
#undef VGL_FIT_XY_PARABOLOID_3D_INSTANTIATE
#define VGL_FIT_XY_PARABOLOID_3D_INSTANTIATE(T) template class vgl_fit_xy_paraboloid_3d<T>

#endif // vgl_fit_xy_paraboloid_3d_hxx_
