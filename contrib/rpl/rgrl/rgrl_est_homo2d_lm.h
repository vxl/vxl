#ifndef rgrl_est_homo2d_lm_h_
#define rgrl_est_homo2d_lm_h_

//:
// \file
// \author Gehua Yang
// \date   Nov 2004

#include <rgrl/rgrl_estimator.h>

//: homography2D transform estimator
//
class rgrl_est_homo2d_lm
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  rgrl_est_homo2d_lm( bool with_grad = true );

  //: Estimates homography transformation
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;

  //: Estimates homography transformation
  //
  // \sa rgrl_estimator::estimate
  //
  // The estimation technique is the normalized DLT (Direct Linear
  // Transformation) algorithm.
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_homo2d_lm, rgrl_estimator );

private:

  bool with_grad_;
};

#endif 
