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
#pragma implementation "TripleMatchSetCorner.h"
#endif
//
// Class: TripleMatchSetCorner
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 09 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include "TripleMatchSetCorner.h"

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_vector.h>
#include <mvl/HomgInterestPointSet.h>
#include <mvl/PairMatchSetCorner.h>

static void set_size(vcl_vector<HomgPoint2D>& v, unsigned n)
{
  v.resize(n);
}


TripleMatchSetCorner::TripleMatchSetCorner():
  TripleMatchSet(0,0)
{
}

// -- Construct a TripleMatchSetCorner that is associated with the given HomgInterestPointSets.
TripleMatchSetCorner::TripleMatchSetCorner(HomgInterestPointSet const* lines1,
					   HomgInterestPointSet const* lines2,
					   HomgInterestPointSet const* lines3):
  TripleMatchSet(new PairMatchSetCorner(lines1, lines2),
		 new PairMatchSetCorner(lines2, lines3))
{
}

// -- Construct a TripleMatchSetCorner from two pairwise match sets.
TripleMatchSetCorner::TripleMatchSetCorner(const PairMatchSetCorner& match12,
					   const PairMatchSetCorner& match23):
  TripleMatchSet(new PairMatchSetCorner(match12.get_corners1(), match12.get_corners2()),
		 new PairMatchSetCorner(match23.get_corners1(), match23.get_corners2()))
{
  assert(match23.get_corners1() == match12.get_corners2());
  set_from_pairwise_matches(match12, match23);
}

// -- Copy ctor
TripleMatchSetCorner::TripleMatchSetCorner(const TripleMatchSetCorner& that):
  TripleMatchSet(new PairMatchSetCorner(that.get_corners1(), that.get_corners2()),
		 new PairMatchSetCorner(that.get_corners2(), that.get_corners3()))
{
  set_from_pairwise_matches(*that._match12, *that._match23);
}
  


// -- Copy the inliers from the TripleMatchSetCorner into the given arrays
// of corners and corner indices.
void TripleMatchSetCorner::extract_matches(vcl_vector<HomgPoint2D> &points1, vcl_vector<int> &indices1,
					   vcl_vector<HomgPoint2D> &points2, vcl_vector<int> &indices2,
					   vcl_vector<HomgPoint2D> &points3, vcl_vector<int> &indices3) const 
{
  const HomgInterestPointSet* corners1 = get_corners1();
  const HomgInterestPointSet* corners2 = get_corners2();
  const HomgInterestPointSet* corners3 = get_corners3();

  unsigned size = count();
  set_size(points1, size);
  set_size(points2, size);
  set_size(points3, size);

  indices1.resize(size);
  indices2.resize(size);
  indices3.resize(size);
  
  unsigned i = 0;
  for(iterator p = begin(); p; ++p, ++i) {
    indices1[i] = p.get_i1();
    indices2[i] = p.get_i2();
    indices3[i] = p.get_i3();
    points1[i] = corners1->get_homg(indices1[i]);
    points2[i] = corners2->get_homg(indices2[i]);
    points3[i] = corners3->get_homg(indices3[i]);
  }
}

// -- Copy the inliers from the TripleMatchSetCorner into the given arrays.
void TripleMatchSetCorner::extract_matches(vcl_vector <HomgPoint2D>& points1,
					   vcl_vector <HomgPoint2D>& points2,
					   vcl_vector <HomgPoint2D>& points3) const
{
  const HomgInterestPointSet* corners1 = get_corners1();
  const HomgInterestPointSet* corners2 = get_corners2();
  const HomgInterestPointSet* corners3 = get_corners3();

  unsigned size = count();
  set_size(points1, size);
  set_size(points2, size);
  set_size(points3, size);

  int i = 0;
  for(iterator p = begin(); p; ++p, ++i) {
    points1[i] = corners1->get_homg(p.get_i1());
    points2[i] = corners2->get_homg(p.get_i2());
    points3[i] = corners3->get_homg(p.get_i3());
  }
}

#ifdef MAIN
main()
{
  HomgInterestPointSet c1;
  HomgInterestPointSet c2;
  HomgInterestPointSet c3;

  c1.add(1, 1, 0);
  c1.add(1, 2, 0);
  c1.add(1, 3, 0);
  c1.add(1, 4, 0);

  c2.add(2, 1, 0);
  c2.add(2, 2, 0);
  c2.add(2, 3, 0);
  c2.add(2, 4, 0);

  c3.add(3, 1, 0);
  c3.add(3, 2, 0);
  c3.add(3, 3, 0);
  c3.add(3, 4, 0);
  
  TripleMatchSetCorner fred(c1, c2, c3);
  fred.add_match(1,2,1);
  fred.add_match(2,1,2);
  fred.add_match(3,3,3);

  vcl_vector<HomgPoint2D> p1;
  vcl_vector<HomgPoint2D> p2;
  vcl_vector<HomgPoint2D> p3;

  cout << "count  = " << fred.count() << endl;
  
  fred.extract_matches(p1, p2, p3);

  cout << p1 << endl;
}
#endif
