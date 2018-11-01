// This is oxl/mvl/TripleMatchSetCorner.h
#ifndef TripleMatchSetCorner_h_
#define TripleMatchSetCorner_h_
//:
//  \file
// \brief Three-view corner matches
//
//    TripleMatchSetCorner is a TripleMatchSet that holds matches
//    between corners in three HomgInterestPointSets.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// \verbatim
//  Modifications:
//     260297 AWF Added documentation
// \endverbatim
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSetCorner.h>
#include "TripleMatchSet.h"

class TripleMatchSetCorner : public TripleMatchSet
{
 public:

  TripleMatchSetCorner();
  TripleMatchSetCorner(HomgInterestPointSet const* corners1,
                       HomgInterestPointSet const* corners2,
                       HomgInterestPointSet const* corners3);

  TripleMatchSetCorner(const TripleMatchSetCorner& that);
  TripleMatchSetCorner& operator=(const TripleMatchSetCorner& that);

  //: Copy triplet matches out of two pairwise match sets.
  TripleMatchSetCorner(const PairMatchSetCorner& matches12, const PairMatchSetCorner& matches23);

  //: Copy inliers to three arrays, and record the original indices.
  void extract_matches(std::vector <HomgPoint2D>& points1, std::vector <int>& corner_index_1,
                       std::vector <HomgPoint2D>& points2, std::vector <int>& corner_index_2,
                       std::vector <HomgPoint2D>& points3, std::vector <int>& corner_index_3) const;

  //: Copy inliers to three arrays
  void extract_matches(std::vector <HomgPoint2D>& points1,
                       std::vector <HomgPoint2D>& points2,
                       std::vector <HomgPoint2D>& points3) const;

  PairMatchSetCorner* get_matches12() { return (PairMatchSetCorner*)match12_; }
  PairMatchSetCorner* get_matches23() { return (PairMatchSetCorner*)match23_; }
  PairMatchSetCorner const* get_matches12() const { return (PairMatchSetCorner const*)match12_; }
  PairMatchSetCorner const* get_matches23() const { return (PairMatchSetCorner const*)match23_; }

  //HomgInterestPointSet* get_corners1() { return get_matches12()->get_corners1(); }
  //HomgInterestPointSet* get_corners2() { return get_matches12()->get_corners2(); }
  //HomgInterestPointSet* get_corners3() { return get_matches23()->get_corners2(); }
  HomgInterestPointSet const* get_corners1() const { return get_matches12()->get_corners1(); }
  HomgInterestPointSet const* get_corners2() const { return get_matches12()->get_corners2(); }
  HomgInterestPointSet const* get_corners3() const { return get_matches23()->get_corners2(); }
};

#endif // TripleMatchSetCorner_h_
