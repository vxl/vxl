#ifndef rgrl_weighter_h_
#define rgrl_weighter_h_

//:
// \file
// \brief  Base class to compute weights.  Main function is pure virtual.
// \author Chuck Stewart
// \date   25 Nov 2002

#include "rgrl_scale_sptr.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_object.h"

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

  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter,  rgrl_object);
};

#endif
