// This is oxl/mvl/PairSetCorner.h
#ifndef PairSetCorner_h_
#define PairSetCorner_h_
//:
// \file
// \brief Copy matches out of PairMatchSetCorner
//
//    Extract the matches from a PairMatchSetCorner.  Copies
//    out the inliers into two arrays of HomgPoint2D, and remembers the indices.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Sep 96
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgPoint2D.h>
#include <mvl/PairMatchSetCorner.h>

class PairSetCorner
{
 public:
  std::vector<HomgPoint2D> points1;
  std::vector<HomgPoint2D> points2;

  // Constructors/Destructors--------------------------------------------------
  PairSetCorner(const PairMatchSetCorner& matches);
};

#endif // PairSetCorner_h_
