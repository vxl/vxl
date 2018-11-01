// This is oxl/mvl/PairMatchSetLineSeg.cxx
//:
//  \file

#include "PairMatchSetLineSeg.h"

#include <mvl/MatchSet.h>
#include <mvl/LineSegSet.h>

PairMatchSetLineSeg::PairMatchSetLineSeg()
{
  linesegs1_ = nullptr;
  linesegs2_ = nullptr;
}

PairMatchSetLineSeg::PairMatchSetLineSeg(LineSegSet& linesegs1, LineSegSet& linesegs2):
  PairMatchSet(linesegs1.size())
{
  linesegs1_ = &linesegs1;
  linesegs2_ = &linesegs2;
}

PairMatchSetLineSeg::~PairMatchSetLineSeg() = default;

//: Extract the point vectors for only the valid matches.
void PairMatchSetLineSeg::extract_matches(std::vector <HomgLineSeg2D>& linesegs1,
                                          std::vector <HomgLineSeg2D>& linesegs2) const
{
  unsigned n = count();
  linesegs1.resize(n);
  linesegs2.resize(n);
  int i = 0;
  for (PairMatchSet::iterator match = *this; match; match.next()) {
    linesegs1[i] = linesegs1_->get_homg(match.get_i1());
    linesegs2[i] = linesegs2_->get_homg(match.get_i2());
    ++i;
  }
}


//: Set the pair of LineSegSets to which matches refer.
void PairMatchSetLineSeg::set(LineSegSet* linesegs1, LineSegSet* linesegs2)
{
  linesegs1_ = linesegs1;
  linesegs2_ = linesegs2;
  set_size(linesegs1_->size());
}
