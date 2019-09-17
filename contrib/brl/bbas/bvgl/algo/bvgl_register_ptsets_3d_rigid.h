// This is bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h

#ifndef bvgl_register_ptsets_3d_rigid_h_
#define bvgl_register_ptsets_3d_rigid_h_

//:
// \file
// \brief register two 3-d poinsets with 2-d rotation and 3-d translation
//
// \author J.L. Mundy
// \date Sept. 7, 2019
//
// \verbatim
//  Modifications:None
// \endverbatim


#include <iostream>
#include <ostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <vnl/vnl_random.h>

template <class T>
class bvgl_register_ptsets_3d_rigid
{

public:
  //: Constructor - default
  bvgl_register_ptsets_3d_rigid():
    t_range_(vgl_vector_3d<T>(2.5, 2.5, 2.5)),
    t_inc_(vgl_vector_3d<T>(0.5, 0.5, 0.5)),
    outlier_thresh_(5.0), transform_fraction_(0.05),
    min_n_pts_(1000), min_error_(std::numeric_limits<T>::max()) {}

  bvgl_register_ptsets_3d_rigid(vgl_pointset_3d<T> const& fixed, vgl_pointset_3d<T> const& movable):
    t_range_(vgl_vector_3d<T>(2.5, 2.5, 2.5)),
    t_inc_(vgl_vector_3d<T>(0.5, 0.5, 0.5)),
    outlier_thresh_(5.0), transform_fraction_(0.05),
    min_n_pts_(1000), min_error_(std::numeric_limits<T>::max())
  {
    fixed_ = fixed; movable_ = movable; knn_fixed_ = bvgl_k_nearest_neighbors_3d<T>(fixed);
    unsigned n = movable_.npts();
    size_t nf = static_cast<size_t>(transform_fraction_ * n);
    if (nf < min_n_pts_)
      nf = min_n_pts_;
    if (nf > n)
      nf = n;
    vnl_random rand;
    for (size_t i = 0; i < nf; ++i) {
      size_t k = static_cast<size_t>(rand(n));
      const vgl_point_3d<T>& p = movable_.p(k);
      frac_trans_.add_point(p);
    }
  }
  bool read_fixed_ptset(std::string const& fixed_path);
  bool read_movable_ptset(std::string const& movable_path);
  bool save_transformed_ptset(std::string const& path);
  void set_search_range( vgl_vector_3d<T> const& t_range){ t_range_ = t_range;}
  void set_search_increment( vgl_vector_3d<T> const& t_inc){ t_inc_ = t_inc;}
  void set_outlier_thresh(T outlier_thresh){outlier_thresh_ = outlier_thresh;}
  void set_transform_fraction(T fraction){transform_fraction_ = fraction;}
  void set_min_n_pts(size_t n) { min_n_pts_ = n; }
  T error(vgl_vector_3d<T> const& t);
  bool minimize();
  void sort_tranformed_distances();
  vgl_vector_3d<T> t()const {return t_;}
  T min_error() const {return min_error_;}
  std::vector<std::pair<T, vgl_point_3d<T> > > sorted_distance() const {return sorted_distance_;}

 private:
  static bool dless(std::pair<T, vgl_point_3d<T> > const& a, std::pair<T, vgl_point_3d<T> > const& b);
  T outlier_thresh_;
  vgl_pointset_3d<T> fixed_;
  bvgl_k_nearest_neighbors_3d<T> knn_fixed_;
  vgl_pointset_3d<T> movable_;
  double transform_fraction_;
  size_t min_n_pts_;
  vgl_pointset_3d<T> frac_trans_;
  vgl_vector_3d<T> t_;
  vgl_vector_3d<T> t_range_;
  vgl_vector_3d<T> t_inc_;
  T min_error_;
  std::vector<std::pair<T, vgl_point_3d<T> > > sorted_distance_;
};


#endif // bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h
