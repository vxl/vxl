// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "PairMatchSetCorner.h"
#endif
//
// Class: PairMatchSetCorner
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 09 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "PairMatchSetCorner.h"

#include <vcl/vcl_vector.h>
#include <mvl/HomgInterestPointSet.h>

// -- Constructor
PairMatchSetCorner::PairMatchSetCorner() : _corners1(0) , _corners2(0)
{
}

// -- Construct a PairMatchSetCorner that will contain matches between the
// given HomgInterestPointSets.
// These objects are held by reference in the MatchSet and must therefore
// live longer than the PairMatchSetCorner (for example in an MViewDatabase).
PairMatchSetCorner::PairMatchSetCorner(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2) : _corners1(0) , _corners2(0)
{
  set(corners1, corners2);
}

// -- Copy a PairMatchSetCorner
PairMatchSetCorner::PairMatchSetCorner(const PairMatchSetCorner& that):
  PairMatchSet(that) , _corners1(0) , _corners2(0)
{
  set(that._corners1, that._corners2);
}

PairMatchSetCorner& PairMatchSetCorner::operator=(const PairMatchSetCorner& that)
{
  set(that._corners1, that._corners2);
  PairMatchSet::operator= (that);
  return *this;
}

// -- Destructor
PairMatchSetCorner::~PairMatchSetCorner()
{
}

// Data Control--------------------------------------------------------------

// -- Set the pair of HomgInterestPointSets to which matches refer.
// See the constructor for constraints.
void PairMatchSetCorner::set(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2)
{
  _corners1 = corners1;
  _corners2 = corners2;
  if (_corners1) 
    set_size(_corners1->size());
  else
    set_size(0);
}

// -- Extract the point vectors for only the valid matches.
// For example, given a set of matches between corner features,
// this function copies the inliers to a pair of arrays which
// can then be fed to a non-robust matcher.
void PairMatchSetCorner::extract_matches(vcl_vector <HomgPoint2D>& points1,
					 vcl_vector <HomgPoint2D>& points2) const
{
  int n = count();
  points1.resize(n);
  points2.resize(n);
  int i = 0;
  for(iterator match = *this; match; ++match) {
    points1[i] = _corners1->get_homg(match.get_i1());
    points2[i] = _corners2->get_homg(match.get_i2());
    ++i;
  }
  assert(i == n);
}

// -- Extract the point vectors for only the valid matches.
// In addition, return the corresponding point indices in corner_index_[12].
// Thus, points1[0] = corner_set_1()[corner_index_1[0]].
// This is useful with procedures such as RANSAC.
void PairMatchSetCorner::extract_matches(vcl_vector <HomgPoint2D>& points1,
					 vcl_vector <int>& corner_index_1,
					 vcl_vector <HomgPoint2D>& points2,
					 vcl_vector <int>& corner_index_2) const
{
  unsigned n = count();
  points1.resize(n);
  points2.resize(n);
  corner_index_1.resize(n);
  corner_index_2.resize(n);
  int i = 0;
  for(iterator match = *this; match; match.next()) {
    corner_index_1[i] = match.get_i1();
    corner_index_2[i] = match.get_i2();
    points1[i] = _corners1->get_homg(match.get_i1());
    points2[i] = _corners2->get_homg(match.get_i2());
    ++i;
  }
}

// -- Clear all matches and then set only those for which the corresponding
// inliers flag is set.  For example, if inliers[5] == true, then the match
// (corner_index_1[5], corner_index_2[5]) is added to the set.
void PairMatchSetCorner::set(const vcl_vector<bool>& inliers,
			     const vcl_vector<int>&  corner_index_1,
			     const vcl_vector<int>&  corner_index_2)
{
  clear();
  unsigned n = inliers.size();
  for(unsigned i = 0; i < n; ++i)
    if (inliers[i])
      add_match(corner_index_1[i], corner_index_2[i]);
}
