#ifndef bvgl_register_ptsets_3d_rigid_hxx_
#define bvgl_register_ptsets_3d_rigid_hxx_

#include "bvgl_register_ptsets_3d_rigid.h"
#include <limits>
#include <fstream>
#include <algorithm>
#include <vgl/vgl_distance.h>

template <class T>
bool bvgl_register_ptsets_3d_rigid<T>::dless(std::pair<T, vgl_point_3d<T> > const& a, std::pair<T, vgl_point_3d<T> > const& b)
{
  return a.first < b.first;
}

// rotate then translate
template <class T>
T bvgl_register_ptsets_3d_rigid<T>::error(vgl_vector_3d<T> const& t)
{
  size_t n = frac_trans_.npts();
  T error = T(0);
  T cnt = T(0);
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<T>& p = frac_trans_.p(i);
    vgl_point_3d<T> tp(p.x()+t.x(), p.y()+t.y(), p.z()+t.z());
    vgl_point_3d<T> cp;
    if (!knn_fixed_.closest_point(tp, cp)) {
      std::cout << "KNN index failed to find neighbors" << std::endl;
      return std::numeric_limits<T>::max();
    }
    T d = vgl_distance<T>(tp, cp);
    if (d > outlier_thresh_)
      continue;
    cnt += T(1);
    error += d*d;
  }
  error /= cnt;
  return sqrt(error);
}

template <class T>
bool bvgl_register_ptsets_3d_rigid<T>::minimize()
{
  if (fixed_.npts() == 0 || frac_trans_.npts() == 0) {
    std::cerr << "No points to minimize" << std::endl;
    return false;
  }
  T min_z = -t_range_.z(), max_z = t_range_.z();
  T min_y = -t_range_.y(), max_y = t_range_.y();
  T min_x = -t_range_.x(), max_x = t_range_.x();
  min_error_ = std::numeric_limits<T>::max();
  T z_at_min = T(0), y_at_min = T(0), x_at_min = T(0);
  for (T z = min_z; z <= max_z; z += t_inc_.z()) {
    for (T y = min_y; y <= max_y; y += t_inc_.y()) {
      for (T x = min_x; x <= max_x; x += t_inc_.x()) {
        T err = error(vgl_vector_3d<T>(x, y, z));
        if (err < min_error_) {
          min_error_ = err;
          z_at_min = z;
          y_at_min = y;
          x_at_min = x;
        }
      }
    }
  }
  if (min_error_ >= outlier_thresh_) {
    t_ = vgl_vector_3d<T>(T(0), T(0), T(0));
    return false;
  }
  t_ = vgl_vector_3d<T>(x_at_min, y_at_min, z_at_min);
  return true;
}

template <class T>
void bvgl_register_ptsets_3d_rigid<T>::sort_tranformed_distances()
{
  size_t n = movable_.npts();
  for (size_t i = 0; i<n; ++i)
  {
    const vgl_point_3d<T>& p = movable_.p(i);
    vgl_point_3d<T> tp = p + t_;
    vgl_point_3d<T> cp;
    if (!knn_fixed_.closest_point(tp, cp)) {
      std::cout << "KNN index failed to find neighbors" << std::endl;
      return;
    }
    T d = vgl_distance<T>(tp, cp);
    if (d > outlier_thresh_)
      continue;
    std::pair<T, vgl_point_3d<T> > pr(d, tp);
    sorted_distance_.push_back(pr);
  }
  std::sort(sorted_distance_.begin(), sorted_distance_.end(), dless);
}

template <class T>
bool bvgl_register_ptsets_3d_rigid<T>::read_fixed_ptset(std::string const& fixed_path)
{
  fixed_.clear();
  std::ifstream istr(fixed_path);
  if (!istr) {
    std::cerr << "Can't load fixed pointset from " << fixed_path << std::endl;
    return false;
  }
  istr >> fixed_;
  knn_fixed_ = bvgl_k_nearest_neighbors_3d<T>(fixed_);
  return true;
}

template <class T>
bool bvgl_register_ptsets_3d_rigid<T>::read_movable_ptset(std::string const& movable_path)
{
  movable_.clear();
  std::ifstream istr(movable_path);
  if (!istr) {
    std::cerr << "Can't load transformed pointset from " << movable_path << std::endl;
    return false;
  }
  istr >> movable_;
  unsigned n = movable_.npts();
  size_t nf = static_cast<size_t>(transform_fraction_*n);
  if (nf<min_n_pts_)
    nf = min_n_pts_;
  if (nf > n)
    nf = n;
  vnl_random rand;
  for (size_t i = 0; i<nf; ++i) {
    size_t k = static_cast<size_t>(rand(n));
    const vgl_point_3d<T>& p = movable_.p(k);
    frac_trans_.add_point(p);
  }
  return true;
}

template <class T>
bool bvgl_register_ptsets_3d_rigid<T>::save_transformed_ptset(std::string const& path)
{
  std::ofstream ostr(path);
  if (!ostr) {
    std::cerr << "Can't save transformed pointset to " << path << std::endl;
    return false;
  }
  size_t n = movable_.npts();
  vgl_pointset_3d<T> temp;
  for (size_t i = 0; i<n; ++i) {
    const vgl_point_3d<T>& p = movable_.p(i);
    vgl_point_3d<T> pt = p + t_;
    temp.add_point(pt);
  }
  ostr << temp;
  ostr.close();
  return true;
}

#define BVGL_REGISTER_PTSETS_3D_RIGID_INSTANTIATE(T)    \
template class bvgl_register_ptsets_3d_rigid<T>

#endif //bvgl_register_ptsets_3d_rigid_hxx_
