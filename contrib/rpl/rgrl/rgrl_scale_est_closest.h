#ifndef rgrl_scale_est_closest_h_
#define rgrl_scale_est_closest_h_

//:
// \file
// \brief  Implementation of unweighted scale estimator using the closest of the match points.
// \author Chuck Stewart
// \date   25 Nov 2002

#include "rgrl_scale_estimator.h"

#include <vcl_memory.h>
#include <vnl/vnl_matrix.h>

class rrel_objective;

//: Unweighted scale estimator using closest of the match points.
//
// This can also be used as a weighted scale estimator, albeit one
// that just ignores the weights.
//
// The scale estimator is able to estimate both the geometric scale
// and the signature error in the form of the covariance matrix. The
// latter is optional (do_signature_scale). In practice, if other
// components for the registration engine require the use of
// signature_error_vector, the do_signature_scale should be set to
// true for the constructor.
//
class rgrl_scale_est_closest
  : public rgrl_scale_estimator
{
public:
  //: 
  //\a obj is the objective function that will be used to estimate a
  //robust scale.  The one that is commonly used is the MUSE objective 
  //function. The flag \a do_signature_scale determines whether a signature 
  //covariance will be estimated.
  rgrl_scale_est_closest( vcl_auto_ptr<rrel_objective>  obj,
			  bool                          do_signature_scale = false );

  ~rgrl_scale_est_closest();

  rgrl_scale_sptr
  estimate_unweighted( rgrl_match_set const& match_set,
                       rgrl_scale_sptr const& current_scales,
                       bool penalize_scaling = false ) const;

  rgrl_scale_sptr
  estimate_weighted( rgrl_match_set const& match_set,
                     rgrl_scale_sptr const& current_scales,
                     bool use_signature_only = false,
                     bool penalize_scaling = false) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_scale_est_closest, rgrl_scale_estimator );

private:
  //disabled
  rgrl_scale_est_closest( rgrl_scale_est_closest const& );
  rgrl_scale_est_closest& operator=( rgrl_scale_est_closest const& );

  double
  compute_geometric_scale( rgrl_match_set const& match_set,
                           bool penalize_scaling ) const;
								
  vnl_matrix<double>
  compute_signature_covar( rgrl_match_set const& match_set ) const;

protected:
  bool do_signature_scale_;
  vcl_auto_ptr<rrel_objective> obj_;
};

#endif // rgrl_scale_est_closest_h_
