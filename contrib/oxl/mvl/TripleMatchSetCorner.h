#ifndef TripleMatchSetCorner_h_
#define TripleMatchSetCorner_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME TripleMatchSetCorner - Three-view corner matches
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/TripleMatchSetCorner.h
// .FILE TripleMatchSetCorner.cxx
//
// .SECTION Description
//    TripleMatchSetCorner is a TripleMatchSet that holds matches
//    between corners in three HomgInterestPointSets.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// .SECTION Modifications:
//     260297 AWF Added documentation
//
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSetCorner.h>
#include "TripleMatchSet.h"

class TripleMatchSetCorner : public TripleMatchSet {
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
  void extract_matches(vcl_vector <HomgPoint2D>& points1, vcl_vector <int>& corner_index_1,
                       vcl_vector <HomgPoint2D>& points2, vcl_vector <int>& corner_index_2,
                       vcl_vector <HomgPoint2D>& points3, vcl_vector <int>& corner_index_3) const;

  //: Copy inliers to three arrays
  void extract_matches(vcl_vector <HomgPoint2D>& points1,
                       vcl_vector <HomgPoint2D>& points2,
                       vcl_vector <HomgPoint2D>& points3) const;

  PairMatchSetCorner* get_matches12() { return (PairMatchSetCorner*)_match12; }
  PairMatchSetCorner* get_matches23() { return (PairMatchSetCorner*)_match23; }
  PairMatchSetCorner const* get_matches12() const { return (PairMatchSetCorner const*)_match12; }
  PairMatchSetCorner const* get_matches23() const { return (PairMatchSetCorner const*)_match23; }

  //HomgInterestPointSet* get_corners1() { return get_matches12()->get_corners1(); }
  //HomgInterestPointSet* get_corners2() { return get_matches12()->get_corners2(); }
  //HomgInterestPointSet* get_corners3() { return get_matches23()->get_corners2(); }
  HomgInterestPointSet const* get_corners1() const { return get_matches12()->get_corners1(); }
  HomgInterestPointSet const* get_corners2() const { return get_matches12()->get_corners2(); }
  HomgInterestPointSet const* get_corners3() const { return get_matches23()->get_corners2(); }
};

#endif // TripleMatchSetCorner_h_
