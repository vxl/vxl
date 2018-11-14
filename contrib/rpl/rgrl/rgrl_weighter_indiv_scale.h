#ifndef rgrl_weighter_indiv_scale_h_
#define rgrl_weighter_indiv_scale_h_
//:
// \file
// \brief  Compute weights in the manner described in PAMI GDB-ICP paper
// \author Gehua Yang
// \date   8 March 2006
//
// \verbatim
//  Modifications:
//   8 March 2006  GY:
//   The error residual is modeled as Gaussian Distribution N(0, (f_s*\sigma)^2)
//   Each feature correspondence has its own feature scale f_s
//   When computing the geometric weights or rho function values,
//   this feature scale has to be counted in.
//
//   13 March 2006 - Peter Vanroose - commented out 3 non-implemented virtuals
// \endverbatim

#include <rgrl/rgrl_weighter_m_est.h>

class rgrl_weighter_indiv_scale
  : public rgrl_weighter_m_est
{
 public:
  //:  constructor takes a pointer to M estimator objective function
  rgrl_weighter_indiv_scale( std::unique_ptr<rrel_m_est_obj>  m_est,
                             bool                          use_signature_error,
                             bool                          use_precomputed_signature_wgt = false );

  ~rgrl_weighter_indiv_scale() override;


  void
  compute_weights( rgrl_scale const&  scale,
                   rgrl_match_set&    match_set ) const override;

  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter_indiv_scale, rgrl_weighter_m_est );

  // Auxiliary functions related to the m_est


  double
  aux_sum_rho_values( rgrl_scale const&  scale,
                      rgrl_match_set&    match_set,
                      rgrl_transformation const&  xform) override;

#if 0 // Do not declare these 3 non-implemented virtual functions - they are already implemented in the parent class
  virtual
  double
  aux_sum_weighted_residuals( rgrl_scale const&  scale,
                              rgrl_match_set&    match_set,
                              rgrl_transformation const&  xform );

  virtual
  double
  aux_neg_log_likelihood( rgrl_scale const&  scale,
                          rgrl_match_set&    match_set,
                          rgrl_transformation const&  xform );

  virtual
  double
  aux_avg_neg_log_likelihood( rgrl_scale const&  scale,
                              rgrl_match_set&    match_set,
                              rgrl_transformation const&  xform );
#endif // 0
};

#endif
