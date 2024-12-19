// This is core/vgl/algo/vgl_fit_cylinder_3d.hxx
#ifndef vgl_fit_cylinder_3d_hxx_
#define vgl_fit_cylinder_3d_hxx_
//:
// \file

#include <iostream>
#include <limits>
#include "vgl_fit_cylinder_3d.h"
#include <vgl/algo/vgl_norm_trans_3d.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_det.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_distance.h>

template <class T>
vgl_fit_cylinder_3d<T>::vgl_fit_cylinder_3d(std::vector<vgl_point_3d<T>> points)

{
  for (typename std::vector<vgl_point_3d<T>>::iterator pit = points.begin(); pit != points.end(); ++pit)
    points_.push_back(vgl_homg_point_3d<T>(*pit));
}

template <class T>
void
vgl_fit_cylinder_3d<T>::add_point(const vgl_point_3d<T> & p)
{
  points_.push_back(vgl_homg_point_3d<T>(p));
}

template <class T>
void
vgl_fit_cylinder_3d<T>::add_point(const T x, const T y, const T z)
{
  points_.push_back(vgl_homg_point_3d<T>(x, y, z));
}

template <class T>
void
vgl_fit_cylinder_3d<T>::clear()
{
  points_.clear();
}

template <class T>
T
vgl_fit_cylinder_3d<T>::fit(const vgl_vector_3d<T> & W, std::ostream * errstream, bool verbose)
{
  const size_t n = points_.size();
  if (!n)
  {
    if (errstream)
      *errstream << "No points to fit cylinder\n";
    return T(-1);
  }
  // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && errstream)
  {
    *errstream << "there is a problem with norm transform\n";
    return T(-1);
  }
  // known orientation, insure unit vector
  vgl_vector_3d<T> Wu = W / W.length();
  cylinder_.set_orientation(Wu);

  // find the center point
  // 1) construct U and V
  vgl_vector_3d<T> Y(T(0), T(1), T(0)), U, V;
  if (dot_product(Y, Wu) > T(0.9))
  {
    vgl_vector_3d<T> X(T(1), T(0), T(0));
    V = cross_product(Wu, X);
    U = cross_product(V, Wu);
  }
  else
  {
    U = cross_product(Y, Wu);
    V = cross_product(Wu, U);
  }
  // 2) compute the covariance matrix of points in U, V coordinates
  //  after norm trans point mean is zero
  T sum_uu = T(0), sum_uv = T(0), sum_vv = T(0);
  T sum_bu = T(0), sum_bv = T(0);
  for (size_t i = 0; i < n; ++i)
  {
    vgl_homg_point_3d<T> hp = norm(points_[i]); // normalize
    T x = hp.x() / hp.w();
    T y = hp.y() / hp.w();
    T z = hp.z() / hp.w();
    vgl_vector_3d<T> X(x, y, z);
    T mu = dot_product(X, U), nu = dot_product(X, V);
    sum_uu += mu * mu;
    sum_uv += mu * nu;
    sum_vv += nu * nu;
    T sq = mu * mu + nu * nu;
    sum_bu += mu * sq;
    sum_bv += nu * sq;
  }
  // 3) solve the 2x2 linear system for the center
  vnl_matrix_fixed<T, 2, 2> covar;
  covar[0][0] = sum_uu / n;
  covar[0][1] = sum_uv / n;
  covar[1][0] = sum_uv / n;
  covar[1][1] = sum_vv / n;
  vnl_vector_fixed<T, 2> cent, B;
  B[0] = sum_bu / n;
  B[1] = sum_bv / n;

  T det = vnl_det(covar);
  if (fabs(det) < T(1.0e-6))
  {
    if (errstream)
      *errstream << "Singular solution for cylinder center \n";
    return T(-1);
  }
  vnl_matrix_fixed<T, 2, 2> covar_inv = vnl_inverse(covar);
  cent = covar_inv * B;
  vgl_vector_3d<T> c3d = cent[0] * U + cent[1] * V;

  // 4) undo the normalizing transformation
  vnl_matrix_fixed<T, 4, 4> H = norm.get_matrix();
  T scale = H[0][0];
  T tx = H[0][3];
  T ty = H[1][3];
  T tz = H[2][3];

  T cx = (c3d.x() - tx) / scale, cy = (c3d.y() - ty) / scale, cz = (c3d.z() - tz) / scale;
  cylinder_.set_center(vgl_point_3d<T>(cx, cy, cz));

  // 5) solve for the radius
  //  compute the projection matrix for W
  vnl_matrix_fixed<T, 3, 1> w;
  w[0][0] = W.x(), w[1][0] = W.y(), w[2][0] = W.z();
  vnl_matrix_fixed<T, 3, 3> P, I, temp;
  I.set_identity();
  temp = w * w.transpose();
  P = I - temp;
  vnl_matrix_fixed<T, 3, 1> C;
  C[0][0] = cx;
  C[1][0] = cy;
  C[2][0] = cz;
  T sum_risq = 0.0;
  for (size_t i = 0; i < n; ++i)
  {
    vnl_matrix_fixed<T, 3, 1> Xi;
    vgl_point_3d<T> pi(points_[i]);
    Xi[0][0] = pi.x();
    Xi[1][0] = pi.y();
    Xi[2][0] = pi.z();
    sum_risq += ((C - Xi).transpose() * P * (C - Xi))[0][0];
  }
  T r2 = sum_risq / n;
  if (r2 < T(0))
  {
    if (errstream)
      *errstream << "Negative squared radius - impossible result \n";
    return T(-1);
  }
  T r = std::sqrt(r2);
  cylinder_.set_radius(r);
  // infinite cylinder
  cylinder_.set_length(std::numeric_limits<T>::max());

  // compute average distance error
  double dsum = 0.0;
  for (size_t i = 0; i < n; i++)
  {
    vgl_point_3d<T> p(points_[i]);
    double d = vgl_distance(p, cylinder_);
    dsum += d;
  }
  return static_cast<T>(dsum / n);
}

template <class T>
std::vector<vgl_point_3d<T>>
vgl_fit_cylinder_3d<T>::get_points() const
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
bool
vgl_fit_cylinder_3d<T>::fit(const vgl_vector_3d<T> & W, const T error_marg, std::ostream * outstream, bool verbose)
{
  T error = fit(W, outstream, verbose);
  return (error < error_marg);
}

template <class T>
T
vgl_fit_cylinder_3d<T>::fit(std::ostream * outstream, bool verbose)
{
  // estimate W from the pointset
  // normalize the points
  vgl_norm_trans_3d<T> norm;
  if (!norm.compute_from_points(points_) && outstream)
  {
    *outstream << "there is a problem with norm transform\n";
    return T(-1);
  }
  // compute the covariance matrix of the points
  size_t n = points_.size();
  T mean_x = T(0), mean_y = T(0), mean_z = T(0);
  for (size_t i = 0; i < n; ++i)
  {
    vgl_point_3d<T> p(points_[i]);
    mean_x += p.x();
    mean_y += p.y();
    mean_z += p.z();
  }
  mean_x /= n;
  mean_y /= n;
  mean_z /= n;

  T sxx = T(0), sxy = T(0), sxz = T(0);
  T syy = T(0), syz = T(0);
  T szz = T(0);
  for (size_t i = 0; i < n; ++i)
  {
    vgl_point_3d<T> p(points_[i]);
    T x = p.x(), y = p.y(), z = p.z();
    sxx += (x - mean_x) * (x - mean_x);
    sxy += (x - mean_x) * (y - mean_y);
    sxz += (x - mean_x) * (z - mean_z);
    syy += (y - mean_y) * (y - mean_y);
    syz += (y - mean_y) * (z - mean_z);
    szz += (z - mean_y) * (z - mean_z);
  }
  sxx /= n;
  sxy /= n;
  sxz /= n;
  syy /= n;
  syz /= n;
  szz /= n;
  vnl_matrix<T> M(3, 3, T(0));
  M[0][0] = sxx;
  M[0][1] = sxy;
  M[0][2] = sxz;
  M[1][0] = M[0][1];
  M[1][1] = syy;
  M[1][2] = syz;
  M[2][0] = M[0][2];
  M[2][1] = M[1][2];
  M[2][2] = szz;
  vnl_symmetric_eigensystem<T> se(M);
  T condition = se.get_eigenvalue(2) / se.get_eigenvalue(0);
  if (verbose && outstream)
    *outstream << "Condition number for W " << condition << std::endl;
  vnl_vector<T> max_eigenvector = se.get_eigenvector(2);
  vgl_vector_3d<T> W(max_eigenvector[0], max_eigenvector[1], max_eigenvector[2]);
  return fit(W, outstream, verbose);
}
template <class T>
bool
vgl_fit_cylinder_3d<T>::fit(const T error_marg, std::ostream * outstream, bool verbose)
{
  T error = fit(outstream, verbose);
  return (error < error_marg);
}
//--------------------------------------------------------------------------
#undef VGL_FIT_CYLINDER_3D_INSTANTIATE
#define VGL_FIT_CYLINDER_3D_INSTANTIATE(T) template class vgl_fit_cylinder_3d<T>

#endif // vgl_fit_cylinder_3d_hxx_
