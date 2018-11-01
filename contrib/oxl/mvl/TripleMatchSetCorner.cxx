// This is oxl/mvl/TripleMatchSetCorner.cxx
//:
//  \file

#include <iostream>
#include <vector>
#include "TripleMatchSetCorner.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgInterestPointSet.h>
#include <mvl/PairMatchSetCorner.h>

static void set_size(std::vector<HomgPoint2D>& v, unsigned n)
{
  v.resize(n);
}


TripleMatchSetCorner::TripleMatchSetCorner():
  TripleMatchSet(nullptr,nullptr)
{
}

//: Construct a TripleMatchSetCorner that is associated with the given HomgInterestPointSets.
TripleMatchSetCorner::TripleMatchSetCorner(HomgInterestPointSet const* lines1,
                                           HomgInterestPointSet const* lines2,
                                           HomgInterestPointSet const* lines3):
  TripleMatchSet(new PairMatchSetCorner(lines1, lines2),
                 new PairMatchSetCorner(lines2, lines3))
{
}

//: Construct a TripleMatchSetCorner from two pairwise match sets.
TripleMatchSetCorner::TripleMatchSetCorner(const PairMatchSetCorner& match12,
                                           const PairMatchSetCorner& match23):
  TripleMatchSet(new PairMatchSetCorner(match12.get_corners1(), match12.get_corners2()),
                 new PairMatchSetCorner(match23.get_corners1(), match23.get_corners2()))
{
  assert(match23.get_corners1() == match12.get_corners2());
  set_from_pairwise_matches(match12, match23);
}

//: Copy ctor
TripleMatchSetCorner::TripleMatchSetCorner(const TripleMatchSetCorner& that):
  TripleMatchSet(new PairMatchSetCorner(that.get_corners1(), that.get_corners2()),
                 new PairMatchSetCorner(that.get_corners2(), that.get_corners3()))
{
  set_from_pairwise_matches(*that.match12_, *that.match23_);
}


//: Copy the inliers from the TripleMatchSetCorner into the given arrays of corners and corner indices.
void TripleMatchSetCorner::extract_matches(std::vector<HomgPoint2D> &points1, std::vector<int> &indices1,
                                           std::vector<HomgPoint2D> &points2, std::vector<int> &indices2,
                                           std::vector<HomgPoint2D> &points3, std::vector<int> &indices3) const
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
  for (iterator p = begin(); p; ++p, ++i) {
    indices1[i] = p.get_i1();
    indices2[i] = p.get_i2();
    indices3[i] = p.get_i3();
    points1[i] = corners1->get_homg(indices1[i]);
    points2[i] = corners2->get_homg(indices2[i]);
    points3[i] = corners3->get_homg(indices3[i]);
  }
}

//: Copy the inliers from the TripleMatchSetCorner into the given arrays.
void TripleMatchSetCorner::extract_matches(std::vector <HomgPoint2D>& points1,
                                           std::vector <HomgPoint2D>& points2,
                                           std::vector <HomgPoint2D>& points3) const
{
  const HomgInterestPointSet* corners1 = get_corners1();
  const HomgInterestPointSet* corners2 = get_corners2();
  const HomgInterestPointSet* corners3 = get_corners3();

  unsigned size = count();
  set_size(points1, size);
  set_size(points2, size);
  set_size(points3, size);

  int i = 0;
  for (iterator p = begin(); p; ++p, ++i) {
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

  std::vector<HomgPoint2D> p1;
  std::vector<HomgPoint2D> p2;
  std::vector<HomgPoint2D> p3;

  std::cerr << "count  = " << fred.count() << std::endl;

  fred.extract_matches(p1, p2, p3);

  std::cerr << p1 << std::endl;
}
#endif
