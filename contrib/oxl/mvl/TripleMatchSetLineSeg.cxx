// This is oxl/mvl/TripleMatchSetLineSeg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include <vcl_cassert.h>

#include <vcl_vector.h>
#include <mvl/HomgLineSeg2D.h>

#include <mvl/LineSegSet.h>

#include "PairMatchSetLineSeg.h"
#include "TripleMatchSetLineSeg.h"


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

void TripleMatchSetLineSeg::extract_matches(vcl_vector<HomgLineSeg2D> &linesegs1, vcl_vector<int> &indices1,
                                            vcl_vector<HomgLineSeg2D> &linesegs2, vcl_vector<int> &indices2,
                                            vcl_vector<HomgLineSeg2D> &linesegs3, vcl_vector<int> &indices3) const
{
  const LineSegSet* _linesegs1 = get_linesegs1();
  const LineSegSet* _linesegs2 = get_linesegs2();
  const LineSegSet* _linesegs3 = get_linesegs3();

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
    linesegs1[i] = _linesegs1->get_homg(indices1[i]);
    linesegs2[i] = _linesegs2->get_homg(indices2[i]);
    linesegs3[i] = _linesegs3->get_homg(indices3[i]);
  }
}
