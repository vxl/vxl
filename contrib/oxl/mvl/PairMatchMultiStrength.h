#ifndef PairMatchMultiStrength_h_
#define PairMatchMultiStrength_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
//  \file
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//-----------------------------------------------------------------------------

//: not implemented

class PairMatchMultiStrength : public PairMatchMulti {
public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchMultiStrength();
  PairMatchMultiStrength(const PairMatchMultiStrength& that);
 ~PairMatchMultiStrength();

  PairMatchMultiStrength& operator=(const PairMatchMultiStrength& that);
};

#endif // PairMatchMultiStrength_h_
