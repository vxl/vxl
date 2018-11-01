// This is oxl/mvl/PairMatchMultiStrength.h
#ifndef PairMatchMultiStrength_h_
#define PairMatchMultiStrength_h_
//:
//  \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//-----------------------------------------------------------------------------

#include "PairMatchMulti.h"

//: not implemented
class PairMatchMultiStrength : public PairMatchMulti
{
 public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchMultiStrength();
  PairMatchMultiStrength(const PairMatchMultiStrength& that);
 ~PairMatchMultiStrength();

  PairMatchMultiStrength& operator=(const PairMatchMultiStrength& that);
};

#endif // PairMatchMultiStrength_h_
