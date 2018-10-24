#ifndef rgrl_est_reduced_quad2d_h_
#define rgrl_est_reduced_quad2d_h_
//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_estimator.h"

//: 2D similarity transform estimator
//
class rgrl_est_reduced_quad2d
  : public rgrl_linear_estimator
{
 public:
  //: Default constructor
  //
  // Does nothing. This constructor is adequate for algorithms that
  // call \a estimate(.) function only. An example of the algorithm is
  // the rgrl_feature_based_registration.
  explicit rgrl_est_reduced_quad2d();

  //: Constructor
  // Takes the dimension of the dataset that the
  // estimator will be applied on, and maybe the minimum number of
  // samples to instantiate a fit. If \a num_samples_for_fit is not
  // set, the number is automatically generated, assuming each
  // component provides one constraint.
  //
  rgrl_est_reduced_quad2d( unsigned int dimension,
                           double condition_num_thrd = 0.0 );

  //: Estimates a 2D reduced_quadratic transform.
  //
  // The return pointer is to a rgrl_trans_reduced_quad2d object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const override;


  //: Estimates a 2D reduced_quadratic transform.
  //
  // The return pointer is to a rgrl_trans_reduced_quad2d object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const override;

  //: Type of transformation estimated by this estimator.
  const std::type_info& transformation_type() const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_reduced_quad2d, rgrl_linear_estimator );

 private:
  double condition_num_thrd_;
};

#endif // rgrl_est_reduced_quad2d_h_
