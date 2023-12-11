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
#include <vgl/vgl_quadric_3d.h>
#include <bvgl/bvgl_k_nearest_neighbors_3d.h>
#include <vnl/vnl_random.h>
// a display of progress as a moving '+' symbol
class reg_progress{
public:
 reg_progress(size_t n_steps):n_(n_steps), move_(1), pos_(1){
    display_ = std::string(n_+2,' ');
    display_[0]='['; display_[n_+1] = ']';
  }
  void operator ()(){
    for(size_t i = 1; i<=n_; ++i)
      display_[i] = ' ';
    display_[pos_]='+';
    std::cout << '\r' << display_ << std::flush;
    pos_ += move_;
    if(pos_>n_){
      pos_ = n_;
      move_ = -move_;
    }
    if(pos_<1){
      pos_ = 1;
      move_ = -move_;
    }
  }
private:
  std::string display_;
  int move_;
  int pos_;
  int n_;
};
template <class T>
class bvgl_register_ptsets_3d_rigid
{

public:
  //: Constructor - default
  bvgl_register_ptsets_3d_rigid() = default;

  //: constructor with pointsets
 bvgl_register_ptsets_3d_rigid(vgl_pointset_3d<T> const& fixed, vgl_pointset_3d<T> const& movable,
                               size_t n_hypos = 100, double transform_fraction = 0.25) :
  fixed_(fixed),
    movable_(movable),
    knn_fixed_(fixed),
    n_hypos_(n_hypos),
    transform_fraction_(transform_fraction)
  {
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
  size_t n_search_pts() const {return frac_trans_.size();}
  const bvgl_k_nearest_neighbors_3d<T>& knn_fixed() const {return knn_fixed_;}
  vgl_vector_3d<T> exhaustive_t()const {return exhaustive_t_;}
  vgl_vector_3d<T> best_ransac_t()const {return best_ransac_t_;}
  vgl_vector_3d<T> analytic_t() const {return analytic_t_;}
  T min_exhaustive_error() const {return min_exhaustive_error_;}
  T min_ransac_error() const {return min_ransac_error_;}
  T min_analytic_error() const {return min_analytic_error_;}
  T error_variance() const {return error_variance_;}

  //: the grid points used for minimize analytic
  std::vector<vgl_point_3d<T> > analytic_grid_pts() const {return analytic_grid_pts_;}

  //: the coefficients after fitting quadric patch to grid points
  vgl_quadric_3d<T> paraboloid_linear() const {return paraboloid_linear_;}
  


  //: the rms error between the movable and fixed closest points
  T error(vgl_vector_3d<T> const& t);

  //: error defined in terms of distances to population fractions
  T distr_error(vgl_vector_3d<T> const& t);

  //: error variance movable and fixed closest points
  T error_var(vgl_vector_3d<T> const& t);

  //: the mean error vector
  vgl_vector_3d<T>  mean_error(vgl_vector_3d<T> const& t);

   //: Find the transformation that minimizes rms error by exhaustive search
  bool minimize_exhaustive();

  //: Find the transformation by a fixed number of RANSAC trials.
  bool minimize_ransac(vgl_vector_3d<T> const& initial_t);

  //: Find the transformation by a fixed number of RANSAC trails, using the exhaustive result as an initial transform.
  bool minimize_ransac() { return minimize_ransac(exhaustive_t()); }

  //: Find the transformation that minimizes distribution error analytically
  // The analytic solution is the extremum of a xy paraboloid fit to
  // a grid of xy positions and error values defined by t_range and t_inc
  bool minimize_analytic(vgl_vector_3d<T> const& initial_t);

  //: Adjust z translation to minimize mean vertical error
  // modifies tz component of analytic translation. tx, ty are analytic min values
  bool minimize_mean_z_error();

  bool minimize_mean_z_error(vgl_vector_3d<T> const& initial_t);

  //: does this instance have valid pointsets
  bool valid_instance() const {return (fixed_.size()>0 && movable_.size()>0);}

 private:
  vgl_vector_3d<T> t_range_ = vgl_vector_3d<T>(T(2.5), T(2.5), T(2.5));
  vgl_vector_3d<T> t_inc_ = vgl_vector_3d<T>(T(0.5), T(0.5), T(0.5));
  T outlier_thresh_ = T(5.0);
  double transform_fraction_ = 0.25;
  size_t min_n_pts_ = 1000;
  size_t n_hypos_ = 100;
  T min_exhaustive_error_ = std::numeric_limits<T>::max();
  T min_ransac_error_ = std::numeric_limits<T>::max();
  T min_analytic_error_ = std::numeric_limits<T>::max();
  T error_variance_ = std::numeric_limits<T>::max();
  vgl_vector_3d<T> exhaustive_t_ = vgl_vector_3d<T>(T(0),T(0),T(0));

  vgl_pointset_3d<T> fixed_;
  bvgl_k_nearest_neighbors_3d<T> knn_fixed_;
  vgl_pointset_3d<T> movable_;
  vgl_pointset_3d<T> frac_trans_;
  vgl_vector_3d<T> best_ransac_t_;
  vgl_vector_3d<T> analytic_t_;

  std::vector<vgl_point_3d<T> > analytic_grid_pts_;
  vgl_quadric_3d<T> paraboloid_linear_;
};


#endif // bbas/bvgl/algo/bvgl_register_ptsets_3d_rigid.h
