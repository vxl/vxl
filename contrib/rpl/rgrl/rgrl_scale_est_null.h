#ifndef rgrl_scale_est_null_h_
#define rgrl_scale_est_null_h_

//:
// \file
// \brief  Implementation of null scale estimator which is set to unit
// \author Charlene Tsai
// \date   25 Nov 2002

#include "rgrl_scale_estimator.h"

//: A null scale estimator that has no effect on the weights
//
class rgrl_scale_est_null
  : public rgrl_scale_estimator
{
public:

  rgrl_scale_est_null( );

  ~rgrl_scale_est_null();

  rgrl_scale_sptr
  estimate_unweighted( rgrl_match_set const& match_set,
                       rgrl_scale_sptr const& current_scales,
                       bool penalize_scaling ) const;

  rgrl_scale_sptr
  estimate_weighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& current_scales,
                     bool use_signature_only,
                     bool penalize_scaling ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale_est_null, rgrl_scale_estimator);

private:
  //disabled
  rgrl_scale_est_null( rgrl_scale_est_null const& );
  rgrl_scale_est_null& operator=( rgrl_scale_est_null const& );

};

#endif // rgrl_scale_est_null_h_
