#ifndef rgrl_est_quadratic_h_
#define rgrl_est_quadratic_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_estimator.h"

//: Quadratic transform estimator
//
class rgrl_est_quadratic
  : public rgrl_estimator
{
public:
  //: Default constructor
  //
  // Does nothing. This constructor is adequate for algorithms that
  // call \a estimate(.) fucntion only. An example of the algorithm is
  // the rgrl_feature_based_registration.
  rgrl_est_quadratic();

  // Constructor which takes the dimension of the dataset that the
  // estimator will be applied on.
  //
  rgrl_est_quadratic( unsigned int dimension,
                      double condition_num_thrd = 0.0);

  //: Estimates an quadratic transform.
  //
  // The return pointer is to a rgrl_trans_quadratic object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
            rgrl_transformation const& cur_transform ) const;


  //: Estimates an quadratic transform.
  //
  // The return pointer is to a rgrl_trans_quadratic object.
  //
  // \sa rgrl_estimator::estimate
  //
  rgrl_transformation_sptr
  estimate( rgrl_match_set_sptr matches,
            rgrl_transformation const& cur_transform ) const;

  //: Type of transformation estimated by this estimator.
  const vcl_type_info& transformation_type() const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_est_quadratic, rgrl_estimator );
  
private:
  //: Return maximum of 9 elements
  double max_of_9_elements(double elt1, double elt2, double elt3, 
                           double elt4, double elt5, double elt6,  
                           double elt7, double elt8, double elt9 ) const;
private:
  double condition_num_thrd_;

};

#endif // rgrl_est_quadratic_h_
