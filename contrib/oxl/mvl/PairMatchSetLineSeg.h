// This is oxl/mvl/PairMatchSetLineSeg.h
#ifndef PairMatchSetLineSeg_h_
#define PairMatchSetLineSeg_h_
//:
// \file
// \brief Matches between homogeneous line segments
//
//    PairMatchSetLineSeg holds matches between homogeneous
//    line segments.  This should be rewritten to use the original
//    IULine*, but this is efficient.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSet.h>
class LineSegSet;
class HomgLineSeg2D;

class PairMatchSetLineSeg : public PairMatchSet
{
  LineSegSet* linesegs1_;
  LineSegSet* linesegs2_;

 public:
  // Constructors/Destructors--------------------------------------------------
  PairMatchSetLineSeg();
  PairMatchSetLineSeg(LineSegSet& corners1, LineSegSet& corners2);
  ~PairMatchSetLineSeg() override;

  // Data Access---------------------------------------------------------------
  LineSegSet* get_linesegs1() const { return linesegs1_; }
  LineSegSet* get_linesegs2() const { return linesegs2_; }

  void extract_matches(std::vector<HomgLineSeg2D>& l1, std::vector<HomgLineSeg2D>& l2) const;

  // Data Control--------------------------------------------------------------
  void set(LineSegSet* lines1, LineSegSet* lines2);
};

#endif // PairMatchSetLineSeg_h_
