// This is oxl/mvl/PairSetCorner.h
#ifndef PairSetCorner_h_
#define PairSetCorner_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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

#include <vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PairMatchSetCorner.h>

class PairSetCorner
{
 public:
  vcl_vector<HomgPoint2D> points1;
  vcl_vector<HomgPoint2D> points2;

  // Constructors/Destructors--------------------------------------------------
  PairSetCorner(const PairMatchSetCorner& matches);
};

#endif // PairSetCorner_h_
