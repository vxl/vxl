#ifndef rgrl_weighter_h_
#define rgrl_weighter_h_
//:
// \file
// \brief  Base class to compute weights.  Main function is pure virtual.
// \author Chuck Stewart
// \date   25 Nov 2002

#include <cassert>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "rgrl_scale_sptr.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_object.h"

class rgrl_transformation;

//: Compute the weights based on the scales and matches.
//
class rgrl_weighter
  : public rgrl_object
{
 public:
  ~rgrl_weighter() override;

  //:  based on the scales, compute the weights for the matches.
  //
  // The resulting weights will be stored in the match_set structure.
  //
  virtual
  void compute_weights( rgrl_scale const&     scales,
                        rgrl_match_set&       match_set ) const = 0;

  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter,  rgrl_object);

  // Auxiliary functions related to the m_est
  virtual double
  aux_sum_weighted_residuals( rgrl_scale const&           /*scale*/,
                              rgrl_match_set&             /*match_set*/,
                              rgrl_transformation const&  /*xform*/ ){ assert(!"NYI"); return 0;}
  virtual double
  aux_sum_rho_values( rgrl_scale const&           /*scale*/,
                      rgrl_match_set&             /*match_set*/,
                      rgrl_transformation const&  /*xform*/){ assert(!"NYI"); return 0 ;}
  virtual double
  aux_neg_log_likelihood( rgrl_scale const&           /*scale*/,
                          rgrl_match_set&             /*match_set*/,
                          rgrl_transformation const&  /*xform*/ ){ assert(!"NYI"); return 0;}
  virtual double
  aux_avg_neg_log_likelihood( rgrl_scale const&  /*scale*/,
                              rgrl_match_set&    /*match_set*/,
                              rgrl_transformation const&  /*xform*/ ){ assert(!"NYI"); return 0;}
};

#endif
