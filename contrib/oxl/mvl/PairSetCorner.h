#ifndef PairSetCorner_h_
#define PairSetCorner_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME PairSetCorner - Copy matches out of PairMatchSetCorner
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/PairSetCorner.h
// .FILE PairSetCorner.cxx
//
// .SECTION Description
//    Extract the matches from a PairMatchSetCorner.  Copies
//    out the inliers into two arrays of HomgPoint2D, and remembers the indices.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 15 Sep 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_vector.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PairMatchSetCorner.h>

class PairSetCorner {
public:
  // Constructors/Destructors--------------------------------------------------

  PairSetCorner(const PairMatchSetCorner& matches);

  vcl_vector<HomgPoint2D> points1;
  vcl_vector<HomgPoint2D> points2;
};

#endif // PairSetCorner_h_
