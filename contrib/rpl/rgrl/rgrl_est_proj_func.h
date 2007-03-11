#ifndef rgrl_est_proj_func_h_
#define rgrl_est_proj_func_h_

//:
// \file
// \author Gehua Yang
// \date   March 2007
// \brief  a generic class to estimate projection matrices using L-M

#include <vnl/vnl_least_squares_function.h>
#include <vnl/vnl_vector_fixed.h>
#include <rgrl/rgrl_fwd.h>
#include <rgrl/rgrl_match_set_sptr.h>

template <unsigned int Fdim, unsigned int Tdim>
class rgrl_est_proj_func
: public vnl_least_squares_function
{
 public:
  //: ctor
  rgrl_est_proj_func( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                      bool with_grad = true );

  // void set_centres( vnl_vector_fixed<double, Fdim> const& fc,
  //                   vnl_vector_fixed<double, Tdim> const& tc )
  // {
  //   from_centre_ = fc;
  //   to_centre_ = tc;
  // }

  //: set max number of iterations
  void set_max_num_iter( int max )
  { max_num_iterations_ = max; }

  //: return max number of iterations
  int max_num_iter() const
  { return max_num_iterations_; }

  //: set relative threshold for parameters change
  void set_rel_thres( double thres )
  { relative_threshold_ = thres; }

  //: relative threshold
  double rel_thres() const
  { return relative_threshold_; }

  //: set zero singular value threshold for SVD
  void set_zero_svd_thres( double thres )
  { zero_svd_thres_ = thres; }

  //: zero singular value threshold
  double zero_svd_thres() const
  { return zero_svd_thres_; }

  //: estimate the projective transformation and the associated covariance
  bool
  projective_estimate( vnl_matrix_fixed<double, Fdim+1, Tdim+1>& proj,
                       vnl_matrix<double>& full_covar,
                       vnl_vector_fixed<double, Fdim>& from_centre,
                       vnl_vector_fixed<double, Tdim>& to_centre );

  //: convert parameters
  void convert_parameters( vnl_vector<double>& params,
                           vnl_matrix_fixed<double, Fdim+1, Tdim+1> proj_matrix,
                           vnl_vector_fixed<double, Fdim> const& fc,
                           vnl_vector_fixed<double, Tdim> const& tc );
  //: convert parameters
  void restored_centered_proj( vnl_matrix_fixed<double, Fdim+1, Tdim+1>& proj_matrix,
                               vnl_vector<double> const& params );

  //: obj func value
  void f(vnl_vector<double> const& x, vnl_vector<double>& fx);

  //: Jacobian
  void gradf(vnl_vector<double> const& x, vnl_matrix<double>& jacobian);

 protected:
  rgrl_set_of<rgrl_match_set_sptr> const* matches_ptr_;
  vnl_vector_fixed<double, Fdim>          from_centre_;
  vnl_vector_fixed<double, Tdim>          to_centre_;

  unsigned int index_row_;
  unsigned int index_col_;

  //: specify the maximum number of iterations for this estimator
  int max_num_iterations_;

  //: The threshold for relative parameter change before termination
  double relative_threshold_;

  //: zero singular value threshold
  double zero_svd_thres_;
};

//: a generic class to estimate projection matrices using L-M
//  including 2D homography, 3D homography, camera matrix
//
// template <unsigned int Fdim, unsigned int Tdim>
// class rgrl_est_proj_lm
//   : public rgrl_nonlinear_estimator
// {
// public:
//   //: Default constructor
//   //
//   rgrl_est_proj_lm( bool with_grad = true );
//
//
//   // Defines type-related functions
//   rgrl_type_macro( rgrl_est_proj_lm, rgrl_nonlinear_estimator );
//
// protected:
//
//   bool with_grad_;
//   //vnl_vector_fixed<double, Fdim> from_camera_centre_;
//   //vnl_vector_fixed<double, Tdim> to_camera_centre_;
// };

#endif //rgrl_est_proj_func_h_
