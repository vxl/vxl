// This is oxl/mvl/PairMatchSetLineSeg.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "PairMatchSetLineSeg.h"

#include <vcl_cstdlib.h>

#include <mvl/MatchSet.h>
#include <mvl/LineSegSet.h>

PairMatchSetLineSeg::PairMatchSetLineSeg()
{
  _linesegs1 = 0;
  _linesegs2 = 0;
}

PairMatchSetLineSeg::PairMatchSetLineSeg(LineSegSet& linesegs1, LineSegSet& linesegs2):
  PairMatchSet(linesegs1.size())
{
  _linesegs1 = &linesegs1;
  _linesegs2 = &linesegs2;
}

PairMatchSetLineSeg::~PairMatchSetLineSeg()
{
}

//: Extract the point vectors for only the valid matches.
void PairMatchSetLineSeg::extract_matches(vcl_vector <HomgLineSeg2D>& linesegs1,
                                          vcl_vector <HomgLineSeg2D>& linesegs2) const
{
  unsigned n = count();
  linesegs1.resize(n);
  linesegs2.resize(n);
  int i = 0;
  for (iterator match = *this; match; match.next()) {
    linesegs1[i] = _linesegs1->get_homg(match.get_i1());
    linesegs2[i] = _linesegs2->get_homg(match.get_i2());
    ++i;
  }
}


//: Set the pair of LineSegSets to which matches refer.
void PairMatchSetLineSeg::set(LineSegSet* linesegs1, LineSegSet* linesegs2)
{
  _linesegs1 = linesegs1;
  _linesegs2 = linesegs2;
  set_size(_linesegs1->size());
}
