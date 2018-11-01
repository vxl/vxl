// This is oxl/mvl/PairMatchSetCorner.cxx
//:
// \file

#include <iostream>
#include <cstdlib>
#include <vector>
#include "PairMatchSetCorner.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgInterestPointSet.h>

//: Constructor
PairMatchSetCorner::PairMatchSetCorner() : corners1_(nullptr) , corners2_(nullptr)
{
}

//: Construct a PairMatchSetCorner that will contain matches between the given HomgInterestPointSets.
// These objects are held by reference in the MatchSet and must therefore
// live longer than the PairMatchSetCorner (for example in an MViewDatabase).
PairMatchSetCorner::PairMatchSetCorner(HomgInterestPointSet const* corners1,
                                       HomgInterestPointSet const* corners2)
  : corners1_(nullptr) , corners2_(nullptr)
{
  set(corners1, corners2);
}

//: Copy a PairMatchSetCorner
PairMatchSetCorner::PairMatchSetCorner(const PairMatchSetCorner& that):
  PairMatchSet(that) , corners1_(nullptr) , corners2_(nullptr)
{
  set(that.corners1_, that.corners2_);
}

PairMatchSetCorner& PairMatchSetCorner::operator=(const PairMatchSetCorner&that)
{
  set(that.corners1_, that.corners2_);
  PairMatchSet::operator= (that);
  return *this;
}

//: Destructor
PairMatchSetCorner::~PairMatchSetCorner() = default;

// Data Control--------------------------------------------------------------

//: Set the pair of HomgInterestPointSets to which matches refer.
// See the constructor for constraints.
void PairMatchSetCorner::set(HomgInterestPointSet const* corners1,
                             HomgInterestPointSet const* corners2)
{
  corners1_ = corners1;
  corners2_ = corners2;
  if (corners1_)
    set_size(corners1_->size());
  else
    set_size(0);
}

//: Extract the point vectors for only the valid matches.
// For example, given a set of matches between corner features,
// this function copies the inliers to a pair of arrays which
// can then be fed to a non-robust matcher.
void PairMatchSetCorner::extract_matches(std::vector<HomgPoint2D>& points1,
                                         std::vector<HomgPoint2D>& points2) const
{
  int n = count();
  points1.resize(n);
  points2.resize(n);
  int i = 0;
  for (PairMatchSet::iterator match = *this; match; ++match) {
    points1[i] = corners1_->get_homg(match.get_i1());
    points2[i] = corners2_->get_homg(match.get_i2());
    ++i;
    if (i > n) {
      std::cerr << "ERRRRRK!";
      std::abort();
    }
  }
  assert(i == n);
}

//: Extract the point vectors for only the valid matches.
// In addition, return the corresponding point indices in corner_index_[12].
// Thus, points1[0] = corner_set_1()[corner_index_1[0]].
// This is useful with procedures such as RANSAC.
void PairMatchSetCorner::extract_matches(std::vector<HomgPoint2D>& points1,
                                         std::vector<int>& corner_index_1,
                                         std::vector<HomgPoint2D>& points2,
                                         std::vector<int>& corner_index_2) const
{
  unsigned n = count();
  points1.resize(n);
  points2.resize(n);
  corner_index_1.resize(n);
  corner_index_2.resize(n);
  int i = 0;
  for (PairMatchSet::iterator match = *this; match; match.next()) {
    corner_index_1[i] = match.get_i1();
    corner_index_2[i] = match.get_i2();
    points1[i] = corners1_->get_homg(match.get_i1());
    points2[i] = corners2_->get_homg(match.get_i2());
    ++i;
  }
}

//: Clear all matches and then set only those for which the corresponding inliers flag is set.
//  For example, if inliers[5] == true, then the match
// (corner_index_1[5], corner_index_2[5]) is added to the set.
void PairMatchSetCorner::set(const std::vector<bool>& inliers,
                             const std::vector<int>&  corner_index_1,
                             const std::vector<int>&  corner_index_2)
{
  clear();
  unsigned n = inliers.size();
  for (unsigned i = 0; i < n; ++i)
    if (inliers[i])
      add_match(corner_index_1[i], corner_index_2[i]);
}
