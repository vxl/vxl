#ifndef rgrl_weighter_m_est_h_
#define rgrl_weighter_m_est_h_
//:
// \file
// \brief  Compute weights in the manner described in the IMCF algorithm
// \author Chuck Stewart
// \date   25 Nov 2002
//
// 20 Sept 2003, CS:  Added the possibility that the intensity and signature
//  weights are pre-computed.  I am not sure that this is the most
//  appropriate way to handle this.  The weighting could be moved
//  into the matching stage, but then the matcher needs to know
//  whether or not to set the weights.
//
// 27 Jan 2004, CT: Intensity is no long considered. The class allows
// the freedom of using the absolute signature weight computed
// somewhere else (\a use_precomputed_signature_wgt ), or the robust
// signature weight from the signature error vector of a match (\a
// use_signature_error ), or neither. \a use_precomputed_signature_wgt
// has precedence over \a use_signature_error.
//

#include <vcl_memory.h>
class rrel_m_est_obj;
class rgrl_transformation;

#include "rgrl_weighter.h"

class rgrl_weighter_m_est
  : public rgrl_weighter
{
 public:
  //:  constructor takes a pointer to M estimator objective function
  rgrl_weighter_m_est( vcl_auto_ptr<rrel_m_est_obj>  m_est,
                       bool                          use_signature_error,
                       bool                          use_precomputed_signature_wgt = false );

  ~rgrl_weighter_m_est();

  void
  compute_weights( rgrl_scale const&  scale,
                   rgrl_match_set&    match_set ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter_m_est, rgrl_weighter );

  // Auxiliary functions related to the m_est
  double 
  aux_sum_weighted_residuals( rgrl_scale const&  scale,
                              rgrl_match_set&    match_set,
                              rgrl_transformation const&  xform );
  double
  aux_sum_rho_values( rgrl_scale const&  scale,
                      rgrl_match_set&    match_set,
                      rgrl_transformation const&  xform);
  double 
  aux_neg_log_likelihood( rgrl_scale const&  scale,
                          rgrl_match_set&    match_set,
                          rgrl_transformation const&  xform );

 protected:
  vcl_auto_ptr<rrel_m_est_obj> m_est_;
  bool use_signature_error_;
  bool signature_precomputed_;
};

#endif


