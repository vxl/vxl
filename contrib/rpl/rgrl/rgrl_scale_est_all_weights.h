#ifndef rgrl_scale_est_all_weights_h_
#define rgrl_scale_est_all_weights_h_

//:
// \file
// \brief  
// \author Chuck Stewart
// \date   6 Dec 2002

#include "rgrl_scale_estimator.h"

#include <vnl/vnl_matrix.h>
class rgrl_match_set;

//: Weighted scale estimator that computes the scale as the sum of the
// weighted residuals divided by the sum of weights. The weights can
// come from either the geometric error, or signature error, or both.
//
// The scale estimator is able to estimate both the geometric scale
// and the signature error in the form of the covariance matrix. The
// latter is optional (\a do_signature_scale ). In practice, if other
// components for the registration engine require the use of
// signature_error_vector, the \a do_signature_scale should be set to
// true for the constructor.
//
class rgrl_scale_est_all_weights
  : public rgrl_scale_estimator_wgted
{
public:
  rgrl_scale_est_all_weights(  bool do_signature_scale = false );

  rgrl_scale_sptr
  estimate_weighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& current_scales,
                     bool use_signature_only =  false,
                     bool penalize_scaling = false) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale_est_all_weights, rgrl_scale_estimator_wgted );

private: 
  double
  compute_geometric_scale( rgrl_match_set const& match_set,bool use_signature_only, bool penalize_scaling ) const;
	  			
  vnl_matrix<double>
  compute_signature_covar( rgrl_match_set const& match_set ) const;

private:
  bool do_signature_scale_;
};

#endif // rgrl_scale_est_all_weights_h_
