#ifndef rgrl_weighter_unit_h_
#define rgrl_weighter_unit_h_

//:
// \file
// \author Amitha Perera
// \date   Feb 2003

#include "rgrl_weighter.h"

//: Assigns unit weight to everything
//
// This is useful for unweighted estimation problems.
//
class rgrl_weighter_unit
  : public rgrl_weighter
{
public:
  //:
  rgrl_weighter_unit();

  //: Assign unit weights to each match
  //
  // The weights will be stored in the match_set structure.
  //
  virtual
  void compute_weights( rgrl_scale const&     scales,
                        rgrl_match_set&       match_set ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_weighter_unit, rgrl_weighter);

};

#endif // rgrl_weighter_unit_h_
