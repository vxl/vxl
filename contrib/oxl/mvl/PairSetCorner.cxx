#ifdef __GNUC__
#pragma implementation
#endif

#include "PairSetCorner.h"

// Default ctor
PairSetCorner::PairSetCorner(const PairMatchSetCorner& matches):
  points1(matches.count()),
  points2(matches.count())
{
  matches.extract_matches(points1, points2);
}
