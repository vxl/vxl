// This is oxl/mvl/TripleMatchSetLineSeg.h
#ifndef TripleMatchSetLineSeg_h_
#define TripleMatchSetLineSeg_h_
//:
// \file
// \brief Three-view line matches
//
//  TripleMatchSetLineSeg is a TripleMatchSet that holds matches
//  between corners in three LineSegSets.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//-----------------------------------------------------------------------------

#include "PairMatchSetLineSeg.h"
#include "TripleMatchSet.h"

class LineSegSet;
class PairMatchSetLineSeg;
class HomgLineSeg2D;

class TripleMatchSetLineSeg : public TripleMatchSet
{
 public:
  TripleMatchSetLineSeg();
  TripleMatchSetLineSeg(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3);
  TripleMatchSetLineSeg(const PairMatchSetLineSeg&, const PairMatchSetLineSeg&);

  void set(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3);

  PairMatchSetLineSeg* get_matches12() { return (PairMatchSetLineSeg*)match12_; }
  PairMatchSetLineSeg* get_matches23() { return (PairMatchSetLineSeg*)match23_; }

  LineSegSet* get_linesegs1() { return get_matches12()->get_linesegs1(); }
  LineSegSet* get_linesegs2() { return get_matches12()->get_linesegs2(); }
  LineSegSet* get_linesegs3() { return get_matches23()->get_linesegs2(); }

  PairMatchSetLineSeg const* get_matches12() const { return (PairMatchSetLineSeg const*)match12_; }
  PairMatchSetLineSeg const* get_matches23() const { return (PairMatchSetLineSeg const*)match23_; }

  LineSegSet const* get_linesegs1() const { return get_matches12()->get_linesegs1(); }
  LineSegSet const* get_linesegs2() const { return get_matches12()->get_linesegs2(); }
  LineSegSet const* get_linesegs3() const { return get_matches23()->get_linesegs2(); }

  //: Copy inliers to three arrays, and record the original indices.
  void extract_matches(std::vector <HomgLineSeg2D>& linesegs1, std::vector <int>& lineseg_index_1,
                       std::vector <HomgLineSeg2D>& linesegs2, std::vector <int>& lineseg_index_2,
                       std::vector <HomgLineSeg2D>& linesegs3, std::vector <int>& lineseg_index_3) const;

  //: Copy inliers to three arrays
  void extract_matches(std::vector <HomgLineSeg2D>& linesegs1,
                       std::vector <HomgLineSeg2D>& linesegs2,
                       std::vector <HomgLineSeg2D>& linesegs3) const;
};

#endif // TripleMatchSetLineSeg_h_
