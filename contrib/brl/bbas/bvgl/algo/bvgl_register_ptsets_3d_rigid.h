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
  vgl_vector_3d<T> search_range() const {return t_range_;}
  vgl_vector_3d<T> search_increment() const {return t_inc_;}
  T outlier_thresh() const { return outlier_thresh_;}
  T transform_fraction() const {return transform_fraction_;}
  size_t min_n_pts() const {return min_n_pts_;}
  bool closest_point(vgl_point_3d<T> const& probe, vgl_point_3d<T>& closest){return knn_fixed_.closest_point(probe, closest);}
  T error(vgl_vector_3d<T> const& t);
  bool minimize();
  vgl_vector_3d<T> t()const {return t_;}
  T min_error() const {return min_error_;}
  const bvgl_k_nearest_neighbors_3d<T>& knn_fixed() const {return knn_fixed_;}
  bool valid_instance() const {return (fixed_.size()>0 && movable_.size()>0);}
 private:
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
};


#endif // bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h
