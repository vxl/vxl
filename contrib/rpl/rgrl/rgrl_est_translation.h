#ifndef rgrl_est_translation_h_
#define rgrl_est_translation_h_

//:
// \file
// \author Charlene Tsai
// \date   Dec 2003

#include "rgrl_estimator.h"

//: Translation transform estimator
//
class rgrl_est_translation
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  // Does nothing. This constructor is adequate for algorithms that
  // call \a estimate(.) fucntion only. An example of the algorithm is
  // the rgrl_feature_based_registration.
  rgrl_est_translation(){}

  // Constructor which takes the dimension of the dataset that the
  // estimator will be applied on.
  rgrl_est_translation( unsigned int dimension );

  //: Estimates an translation transform.
  //
  // The return pointer is to a rgrl_trans_translation object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;


  //: Estimates an translation transform.
  //
  // The return pointer is to a rgrl_trans_translation object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_translation, rgrl_estimator );

};

#endif // rgrl_est_translation_h_
