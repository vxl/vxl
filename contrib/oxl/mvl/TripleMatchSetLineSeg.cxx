// This is oxl/mvl/TripleMatchSetLineSeg.cxx

#include <iostream>
#include <vector>
#include "TripleMatchSetLineSeg.h"

#include <cassert>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mvl/HomgLineSeg2D.h>

#include <mvl/LineSegSet.h>
#include <mvl/PairMatchSetLineSeg.h>


TripleMatchSetLineSeg::TripleMatchSetLineSeg():
  TripleMatchSet(new PairMatchSetLineSeg, new PairMatchSetLineSeg)
{
}

TripleMatchSetLineSeg::TripleMatchSetLineSeg(LineSegSet& lines1,
                                             LineSegSet& lines2,
                                             LineSegSet& lines3):
  TripleMatchSet(new PairMatchSetLineSeg(lines1, lines2),
                 new PairMatchSetLineSeg(lines2, lines3))
{
}

TripleMatchSetLineSeg::TripleMatchSetLineSeg(const PairMatchSetLineSeg& match12,
                                             const PairMatchSetLineSeg& match23):
  TripleMatchSet(new PairMatchSetLineSeg(*match12.get_linesegs1(), *match12.get_linesegs2()),
                 new PairMatchSetLineSeg(*match23.get_linesegs1(), *match23.get_linesegs2()))
{
  assert(match23.get_linesegs1() == match12.get_linesegs2());
  set_from_pairwise_matches(match12, match23);
}

void TripleMatchSetLineSeg::set(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3)
{
  TripleMatchSet::set(new PairMatchSetLineSeg(lines1, lines2), new PairMatchSetLineSeg(lines2, lines3));
}

void TripleMatchSetLineSeg::extract_matches(std::vector<HomgLineSeg2D> &linesegs1, std::vector<int> &indices1,
                                            std::vector<HomgLineSeg2D> &linesegs2, std::vector<int> &indices2,
                                            std::vector<HomgLineSeg2D> &linesegs3, std::vector<int> &indices3) const
{
  const LineSegSet* linesegs1_ = get_linesegs1();
  const LineSegSet* linesegs2_ = get_linesegs2();
  const LineSegSet* linesegs3_ = get_linesegs3();

  int n = count();
  linesegs1.resize(n);
  linesegs2.resize(n);
  linesegs3.resize(n);

  indices1.resize(n);
  indices2.resize(n);
  indices3.resize(n);

  int i = 0;
  for (iterator p = begin(); p; ++p, ++i) {
    indices1[i] = p.get_i1();
    indices2[i] = p.get_i2();
    indices3[i] = p.get_i3();
    linesegs1[i] = linesegs1_->get_homg(indices1[i]);
    linesegs2[i] = linesegs2_->get_homg(indices2[i]);
    linesegs3[i] = linesegs3_->get_homg(indices3[i]);
  }
}
