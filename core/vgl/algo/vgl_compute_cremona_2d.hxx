#include "vgl_compute_cremona_2d.h"
#include "vgl_cremona_trans_2d.h"
#include <cmath>
#include <iostream>
#include <vnl/algo/vnl_svd.h>

template <class T, size_t deg>
bool
vgl_compute_cremona_2d<T, deg>::normalize()
{
  // compute the normalizing transforms
  if (!tr_from_.compute_from_points(from_pts_))
    return false;
  if (!tr_to_.compute_from_points(to_pts_))
    return false;
  size_t n = from_pts_.size();
  if (to_pts_.size() != n)
  {
    std::cerr << "inconsistent number of points, from vs. to" << std::endl;
    return false;
  }
  for (int i = 0; i < n; i++)
  {
    norm_from_pts_.push_back(tr_from_(from_pts_[i]));
    norm_to_pts_.push_back(tr_to_(to_pts_[i]));
  }
  return true;
}
template <class T, size_t deg>
bool
vgl_compute_cremona_2d<T, deg>::compute_linear(const std::vector<vgl_homg_point_2d<T>> & from_pts,
                                               const std::vector<vgl_homg_point_2d<T>> & to_pts,
                                               constraint_t ctype)
{
  constr_type_ = ctype;
  linear_solved_ = false;
  from_pts_ = from_pts;
  to_pts_ = to_pts;
  this->normalize();
  size_t n = from_pts_.size();
  size_t nc = vgl_cremona_trans_2d<T, deg>::n_coeff();
  if (constr_type_ == BI_RATIONAL)
  {
    vnl_matrix<T> A(2 * n, 4 * nc);
    A.fill(T(0));
    for (size_t k = 0; k < n; ++k)
    {
      vgl_point_2d<T> pf(norm_from_pts_[k]), pt(norm_to_pts_[k]);
      T X = pf.x(), Y = pf.y(), x = pt.x(), y = pt.y();
      vnl_vector<T> pv = vgl_cremona_trans_2d<T, deg>::power_vector(X, Y);
      for (size_t i = 0; i < nc; ++i)
      {
        A[2 * k][i] = pv[i];
        A[2 * k][i + nc] = -x * pv[i];
        A[2 * k + 1][i + 2 * nc] = pv[i];
        A[2 * k + 1][i + 3 * nc] = -y * pv[i];
      }
    }
    vnl_svd<T> svd(A);
    size_t r = svd.rank();
    if (r < 4 * nc)
    {
      std::cout << "insufficent rank " << r << " for linear solution of Cremona coefficients" << std::endl;
      return false;
    }
    linear_coeff_ = svd.nullvector();
  }
  else if (constr_type_ == COMMON_DENOMINATOR)
  {
    vnl_matrix<T> A(2 * n, 3 * nc);
    A.fill(T(0));
    for (size_t k = 0; k < n; ++k)
    {
      vgl_point_2d<T> pf(norm_from_pts_[k]), pt(norm_to_pts_[k]);
      T X = pf.x(), Y = pf.y(), x = pt.x(), y = pt.y();
      vnl_vector<T> pv = vgl_cremona_trans_2d<T, deg>::power_vector(X, Y);
      for (size_t i = 0; i < nc; ++i)
      {
        A[2 * k][i] = pv[i];
        A[2 * k][i + nc] = -x * pv[i];
        A[2 * k + 1][i + nc] = -y * pv[i];
        A[2 * k + 1][i + 2 * nc] = pv[i];
      }
    }
    vnl_svd<T> svd(A);
    size_t r = svd.rank();
    if (r < 3 * nc)
    {
      std::cerr << "insufficent rank " << r << " for linear solution of Cremona coefficients" << std::endl;
      return false;
    }
    vnl_vector<T> temp = svd.nullvector();
    vnl_vector<T> neu_x = temp.extract(nc, 0);
    vnl_vector<T> den = temp.extract(nc, nc);
    vnl_vector<T> neu_y = temp.extract(nc, 2 * nc);
    linear_coeff_.set_size(4 * nc);
    linear_coeff_.update(neu_x, 0);
    linear_coeff_.update(den, nc);
    linear_coeff_.update(neu_y, 2 * nc);
    linear_coeff_.update(den, 3 * nc);
  }
  else if (constr_type_ == UNITY_DENOMINATOR)
  {
    vnl_matrix<T> A(2 * n, 2 * nc);
    vnl_vector<T> b(2 * n);
    A.fill(T(0));
    for (size_t k = 0; k < n; ++k)
    {
      vgl_point_2d<T> pf(norm_from_pts_[k]), pt(norm_to_pts_[k]);
      T X = pf.x(), Y = pf.y(), x = pt.x(), y = pt.y();
      vnl_vector<T> pv = vgl_cremona_trans_2d<T, deg>::power_vector(X, Y);
      for (size_t i = 0; i < nc; ++i)
      {
        A[2 * k][i] = pv[i];
        A[2 * k + 1][i + nc] = pv[i];
        b[2 * k] = x;
        b[2 * k + 1] = y;
      }
    }
    vnl_svd<T> svd(A);
    size_t r = svd.rank();
    if (r < 2 * nc)
    {
      std::cerr << "insufficent rank " << r << " for linear solution of Cremona coefficients" << std::endl;
      return false;
    }
    vnl_vector<T> solutn = svd.solve(b);
    vnl_vector<T> neu_x = solutn.extract(nc, 0);
    vnl_vector<T> neu_y = solutn.extract(nc, nc);
    vnl_vector<T> den(nc, 0.0);
    den[0] = T(1);
    linear_coeff_.set_size(4 * nc);
    linear_coeff_.update(neu_x, 0);
    linear_coeff_.update(den, nc);
    linear_coeff_.update(neu_y, 2 * nc);
    linear_coeff_.update(den, 3 * nc);
  }
  else
  {
    std::cerr << "Unknown constraint type" << std::endl;
    return false;
  }

#if 1
  for (size_t k = 0; k < 4; ++k)
  {
    for (size_t i = 0; i < nc; ++i)
      std::cout << linear_coeff_[i + k * nc] << ' ';
    std::cout << std::endl;
  }
#endif
  linear_solved_ = true;
  this->compute_linear_solution_error();
  return true;
}

template <class T, size_t deg>
bool
vgl_compute_cremona_2d<T, deg>::project_linear(T X, T Y, T & x, T & y) const
{
  if (!linear_solved_)
  {
    std::cerr << "no linear solution available" << std::endl;
    return false;
  }
  size_t nc = vgl_cremona_trans_2d<T, deg>::n_coeff();
  vgl_homg_point_2d<T> hX(X, Y), norm_hX = tr_from_(hX);
  vnl_vector<T> pv = vgl_cremona_trans_2d<T, deg>::power_vector(norm_hX.x(), norm_hX.y());
  vnl_vector<T> x_neu = linear_coeff_.extract(nc, 0);
  vnl_vector<T> x_den = linear_coeff_.extract(nc, nc);
  vnl_vector<T> y_neu = linear_coeff_.extract(nc, 2 * nc);
  vnl_vector<T> y_den = linear_coeff_.extract(nc, 3 * nc);
  T px = dot_product(x_neu, pv) / dot_product(x_den, pv);
  T py = dot_product(y_neu, pv) / dot_product(y_den, pv);
  vgl_homg_point_2d<T> hx(px, py), hx_unnorm = tr_to_.preimage(hx);
  vgl_point_2d<T> pu(hx_unnorm);
  x = pu.x();
  y = pu.y();
  return true;
}
template <class T, size_t deg>
bool
vgl_compute_cremona_2d<T, deg>::compute_linear_solution_error()
{
  if (!linear_solved_)
  {
    std::cerr << "no linear solution available" << std::endl;
    return false;
  }
  linear_error_ = T(0);
  size_t n = from_pts_.size();
  for (size_t i = 0; i < n; ++i)
  {
    vgl_point_2d<T> pf(from_pts_[i]);
    vgl_point_2d<T> pt(to_pts_[i]);
    T x = T(0), y = T(0);
    this->project_linear(pf.x(), pf.y(), x, y);
    vgl_point_2d<T> pp(x, y);
    T er = (pt - pp).length();
    linear_error_ += er;
  }
  linear_error_ /= n;
  return true;
}

#undef VGL_COMPUTE_CREMONA_2D_INSTANTIATE
#define VGL_COMPUTE_CREMONA_2D_INSTANTIATE(T, deg) template class vgl_compute_cremona_2d<T, deg>
