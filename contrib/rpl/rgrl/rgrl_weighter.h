#ifndef rgrl_weighter_h_
#define rgrl_weighter_h_

//:
// \file
// \brief  Base class to compute weights.  Main function is pure virtual.
// \author Chuck Stewart
// \date   25 Nov 2002
#include <vcl_cassert.h>

#include "rgrl_scale_sptr.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_object.h"
#include "rgrl_transformation_sptr.h"

//class rgrl_transformation;

//: Compute the weights based on the scales and matches.
//
class rgrl_weighter
  : public rgrl_object
{
public:
  virtual ~rgrl_weighter();

  //:  based on the scales, compute the weights for the matches.
  //
  // The resulting weights will be stored in the match_set structure.
  //
  virtual
  void compute_weights( rgrl_scale const&     scales,
                        rgrl_match_set&       match_set ) const = 0;

  //:  based on the scales, compute the weights for the matches.
  //
  //   The resulting weights will be stored in the match_set structure. 
  //   All the mapped features will be set to NULL, for it is hard to 
  //   keep track on the consistency between weight and mapped features
  // virtual
  // void compute_weights( rgrl_scale const&     scales,
  //                       rgrl_match_set&       match_set ) const = 0;


  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter,  rgrl_object);

  // Auxiliary functions related to the m_est
  virtual double 
  aux_sum_weighted_residuals( rgrl_scale const&  scale,
                              rgrl_match_set&    match_set,
                              rgrl_transformation_sptr const&  xform ){ assert(0); return 0;}
  virtual double
  aux_sum_rho_values( rgrl_scale const&  scale,
                      rgrl_match_set&    match_set,
                      rgrl_transformation_sptr const&  xform){ assert(0); return 0 ;}
  virtual double 
  aux_neg_log_likelihood( rgrl_scale const&  scale,
                          rgrl_match_set&    match_set,
                          rgrl_transformation_sptr const&  xform ){ assert(0); return 0;}
};

#endif
