#ifndef rgrl_est_dis_homo2d_lm_h_
#define rgrl_est_dis_homo2d_lm_h_

//:
// \file
// \author Gehua Yang
// \date   Feb 2005

#include <rgrl/rgrl_estimator.h>
#include <vnl/vnl_double_2.h>

//: homography2D transform estimator
//
//  NOTE:
//    1. assume error projector has all eigen-values as one
// 
class rgrl_est_dis_homo2d_lm
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  rgrl_est_dis_homo2d_lm( vnl_vector<double> const& from_centre,
                          vnl_vector<double> const& to_centre,
                          bool with_grad = true );

  //: Estimates homography transformation
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;

  //: Estimates homography transformation w/ radial lens distortion
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_dis_homo2d_lm, rgrl_estimator );

private:

  vnl_double_2 from_centre_, to_centre_;
  
  bool with_grad_;
};

#endif 
