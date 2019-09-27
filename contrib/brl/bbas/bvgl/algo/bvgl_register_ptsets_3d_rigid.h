// This is bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h

#ifndef bvgl_register_ptsets_3d_rigid_h_
#define bvgl_register_ptsets_3d_rigid_h_

//:
// \file
// \brief register two 3-d pointsets 3-d translation
//
// \author J.L. Mundy
// \date Sept. 7, 2019
//
// \verbatim
//  Modifications:None
// \endverbatim
// Two pointsets are used: 1) a movable pointset and 2) a fixed pointset. The transformation from initial
// movable pointset coordinate system to the fixed pointset is determined. Two methods are currently implmented:
// exhausitve search over a space of translations and a simple form of RANSAC. The fixed pointset is stored in a
// k-d tree to efficiently retrieve the nearest neighbor to a given point from the movable pointset.
// 
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
    outlier_thresh_(5.0), transform_fraction_(0.25),
    min_n_pts_(1000), n_hypos_(100), min_exhaustive_error_(std::numeric_limits<T>::max()),
    min_ransac_error_(std::numeric_limits<T>::max()), exhaustive_t_(vgl_vector_3d<T>(T(0),T(0),T(0))) {}
  //: constructor with pointsets
 bvgl_register_ptsets_3d_rigid(vgl_pointset_3d<T> const& fixed, vgl_pointset_3d<T> const& movable):
  t_range_(vgl_vector_3d<T>(2.5, 2.5, 2.5)),
    t_inc_(vgl_vector_3d<T>(0.5, 0.5, 0.5)),
    outlier_thresh_(5.0), transform_fraction_(0.25),
    min_n_pts_(1000), n_hypos_(100), min_exhaustive_error_(std::numeric_limits<T>::max()),
    min_ransac_error_(std::numeric_limits<T>::max()),exhaustive_t_(vgl_vector_3d<T>(T(0),T(0),T(0)))
  {
    // initialize knn index with fixed pointset
    fixed_ = fixed; movable_ = movable; knn_fixed_ = bvgl_k_nearest_neighbors_3d<T>(fixed);

    // reduce the size of the movable pointset to reduce computation
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
  //: load the fixed pointset (use with default constructor)
  bool read_fixed_ptset(std::string const& fixed_path);

  //: load the movable pointset (use with default constructor)
  bool read_movable_ptset(std::string const& movable_path);

  //: save the transformed pointset
  bool save_transformed_ptset(std::string const& path);

  // ======  Parameters ========
  //: define the bounds for the exhaustive search
  void set_search_range( vgl_vector_3d<T> const& t_range){ t_range_ = t_range;}

  //: define the increment for the exhaustive search
  void set_search_increment( vgl_vector_3d<T> const& t_inc){ t_inc_ = t_inc;}

  //: define a distance threshold for outliers (used in exhaustive search)
  void set_outlier_thresh(T outlier_thresh){outlier_thresh_ = outlier_thresh;}

  //: define the fraction of the movable pointset to be used in computing the transformation
  //  enables reduced computation
  void set_transform_fraction(T fraction){transform_fraction_ = fraction;}

  //: define the least number of points that can be used to compute the transform
  void set_min_n_pts(size_t n) { min_n_pts_ = n; }

  //: the number of transform hypotheses used by RANSAC
  void set_n_hypotheses(size_t n) {n_hypos_ = n;}

  // ===========  Accessors =================
  vgl_vector_3d<T> search_range() const {return t_range_;}
  vgl_vector_3d<T> search_increment() const {return t_inc_;}
  T outlier_thresh() const { return outlier_thresh_;}
  T transform_fraction() const {return transform_fraction_;}
  size_t min_n_pts() const {return min_n_pts_;}
  const bvgl_k_nearest_neighbors_3d<T>& knn_fixed() const {return knn_fixed_;}
  vgl_vector_3d<T> exhaustive_t()const {return exhaustive_t_;}
  vgl_vector_3d<T> best_ransac_t()const {return best_ransac_t_;}
  T min_exhaustive_error() const {return min_exhaustive_error_;}
  T min_ransac_error() const {return min_ransac_error_;}


  //: the rms error between the movable and fixed closest points
  T error(vgl_vector_3d<T> const& t);

  //: error defined in terms of distances to population fractions
  T distr_error(vgl_vector_3d<T> const& t);

  //: Find the transformation that minimizes rms error by exhaustive search
  bool minimize_exhaustive();

  //: Find the transformation by a fixed number of RANSAC trials. Uses distr_error
  // by default use as an intial transform the exhaustive result
  bool minimize_ransac(vgl_vector_3d<T> const& initial_t = exhaustive_t());

  //: does this instance have valid pointsets
  bool valid_instance() const {return (fixed_.size()>0 && movable_.size()>0);}
 private:
  T outlier_thresh_;
  vgl_pointset_3d<T> fixed_;
  bvgl_k_nearest_neighbors_3d<T> knn_fixed_;
  vgl_pointset_3d<T> movable_;
  double transform_fraction_;
  size_t min_n_pts_;
  size_t n_hypos_;
  vgl_pointset_3d<T> frac_trans_;
  vgl_vector_3d<T> best_ransac_t_;
  vgl_vector_3d<T> exhaustive_t_;
  vgl_vector_3d<T> t_range_;
  vgl_vector_3d<T> t_inc_;
  T min_exhaustive_error_;
  T min_ransac_error_;
};


#endif // bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h
