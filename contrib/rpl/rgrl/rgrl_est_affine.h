#ifndef rgrl_est_affine_h_
#define rgrl_est_affine_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_estimator.h"

//: Affine transform estimator
//
class rgrl_est_affine
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  // Does nothing. This constructor is adequate for algorithms that
  // call \a estimate(.) fucntion only. An example of the algorithm is
  // the rgrl_feature_based_registration.
  rgrl_est_affine(){}

  // Constructor which takes the dimension of the dataset that the
  // estimator will be applied on.
  //
  rgrl_est_affine( unsigned int dimension );

  //: Estimates an affine transform.
  //
  // The return pointer is to a rgrl_trans_affine object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;


  //: Estimates an affine transform.
  //
  // The return pointer is to a rgrl_trans_affine object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_affine, rgrl_estimator )

};

#endif // rgrl_est_affine_h_
