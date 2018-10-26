#ifndef rgrl_est_homography2d_h_
#define rgrl_est_homography2d_h_

//:
// \file
// \author Charlene Tsai
// \date   Oct 2004

#include <rgrl/rgrl_estimator.h>

//: homography2D transform estimator
//
class rgrl_est_homography2d
  : public rgrl_linear_estimator
{
 public:
  //: Default constructor
  //
  rgrl_est_homography2d( double condition_num_thrd = 0.0 );

  //: Estimates a quadratic transform.
  //
  // The return pointer is to a rgrl_trans_quadratic object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const override;

  //: Estimates a quadratic transform.
  //
  // The return pointer is to a rgrl_trans_quadratic object.
  //
  // \sa rgrl_estimator::estimate
  //
  // The estimation technique is the normalized DLT (Direct Linear
  // Transformation) algorithm.
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const override;

  //: Type of transformation estimated by this estimator.
  const std::type_info& transformation_type() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_homography2d, rgrl_linear_estimator );

 private:
  bool normalize( rgrl_set_of<rgrl_match_set_sptr> const& matches,
                  std::vector< vnl_vector<double> >& norm_froms,
                  std::vector< vnl_vector<double> >& norm_tos,
                  std::vector< double >& wgts,
                  double& from_scale,
                  double& to_scale,
                  vnl_vector< double > & from_center,
                  vnl_vector< double > & to_center ) const;
  void estimate_covariance( const std::vector< vnl_vector<double> >& norm_froms,
                            const std::vector< vnl_vector<double> >& norm_tos,
                            const std::vector< double >& wgts,
                            double from_scale,
                            double to_scale,
                            vnl_matrix<double>& covar ) const;

 private:
  double condition_num_thrd_;
};

#endif
