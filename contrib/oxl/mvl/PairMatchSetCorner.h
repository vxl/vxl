// This is oxl/mvl/PairMatchSetCorner.h
#ifndef PairMatchSetCorner_h_
#define PairMatchSetCorner_h_
//:
// \file
// \brief Matches between corners
//
//    PairMatchSetCorner is a subclass of PairMatchSet that stores matches
//    between corner features.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// \verbatim
//  Modifications:
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/HomgPoint2D.h>
#include "PairMatchSet.h"

class HomgInterestPointSet;
class PairMatchSetCorner : public PairMatchSet
{
 public:
  // Constructors/Destructors--------------------------------------------------

  PairMatchSetCorner();
  PairMatchSetCorner(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2);
  PairMatchSetCorner(const PairMatchSetCorner& that);
  PairMatchSetCorner& operator=(const PairMatchSetCorner& that);
 ~PairMatchSetCorner() override;

  // Data Access---------------------------------------------------------------
  void extract_matches(std::vector <HomgPoint2D>& points1, std::vector <int>& corner_index_1,
                       std::vector <HomgPoint2D>& points2, std::vector <int>& corner_index_2) const;

  void extract_matches(std::vector <HomgPoint2D>& points1, std::vector <HomgPoint2D>& points2) const;

  //: Clear all matches and then set only those for which the corresponding inliers flag is set.
  void set(const std::vector<bool>& inliers,
           const std::vector<int>&  corner_index_1,
           const std::vector<int>&  corner_index_2);

  // Data Control--------------------------------------------------------------
  void set(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2);

  //: Return the set of corners within which the i1 indices point
  HomgInterestPointSet const* get_corners1() const { return corners1_; }

  //: Return the set of corners within which the i2 indices point
  HomgInterestPointSet const* get_corners2() const { return corners2_; }

 private:
  HomgInterestPointSet const* corners1_;
  HomgInterestPointSet const* corners2_;
};

#endif // PairMatchSetCorner_h_
