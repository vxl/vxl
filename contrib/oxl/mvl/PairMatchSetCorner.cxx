// This is oxl/mvl/PairMatchSetCorner.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "PairMatchSetCorner.h"

#include <vcl_cassert.h>
#include <vcl_cstdlib.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <mvl/HomgInterestPointSet.h>

//: Constructor
PairMatchSetCorner::PairMatchSetCorner() : corners1_(0) , corners2_(0)
{
}

//: Construct a PairMatchSetCorner that will contain matches between the given HomgInterestPointSets.
// These objects are held by reference in the MatchSet and must therefore
// live longer than the PairMatchSetCorner (for example in an MViewDatabase).
PairMatchSetCorner::PairMatchSetCorner(HomgInterestPointSet const* corners1,
                                       HomgInterestPointSet const* corners2)
  : corners1_(0) , corners2_(0)
{
  set(corners1, corners2);
}

//: Copy a PairMatchSetCorner
PairMatchSetCorner::PairMatchSetCorner(const PairMatchSetCorner& that):
  PairMatchSet(that) , corners1_(0) , corners2_(0)
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
PairMatchSetCorner::~PairMatchSetCorner()
{
}

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
void PairMatchSetCorner::extract_matches(vcl_vector<HomgPoint2D>& points1,
                                         vcl_vector<HomgPoint2D>& points2) const
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
      vcl_cerr << "ERRRRRK!";
      vcl_abort();
    }
  }
  assert(i == n);
}

//: Extract the point vectors for only the valid matches.
// In addition, return the corresponding point indices in corner_index_[12].
// Thus, points1[0] = corner_set_1()[corner_index_1[0]].
// This is useful with procedures such as RANSAC.
void PairMatchSetCorner::extract_matches(vcl_vector<HomgPoint2D>& points1,
                                         vcl_vector<int>& corner_index_1,
                                         vcl_vector<HomgPoint2D>& points2,
                                         vcl_vector<int>& corner_index_2) const
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
void PairMatchSetCorner::set(const vcl_vector<bool>& inliers,
                             const vcl_vector<int>&  corner_index_1,
                             const vcl_vector<int>&  corner_index_2)
{
  clear();
  unsigned n = inliers.size();
  for (unsigned i = 0; i < n; ++i)
    if (inliers[i])
      add_match(corner_index_1[i], corner_index_2[i]);
}
