#ifndef PairMatchSetLineSeg_h_
#define PairMatchSetLineSeg_h_
#ifdef __GNUC__
#pragma interface
#endif
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
  LineSegSet* _linesegs1;
  LineSegSet* _linesegs2;

 public:
  // Constructors/Destructors--------------------------------------------------
  PairMatchSetLineSeg();
  PairMatchSetLineSeg(LineSegSet& corners1, LineSegSet& corners2);
  ~PairMatchSetLineSeg();

  // Data Access---------------------------------------------------------------
  LineSegSet* get_linesegs1() const { return _linesegs1; }
  LineSegSet* get_linesegs2() const { return _linesegs2; }

  void extract_matches(vcl_vector<HomgLineSeg2D>& l1, vcl_vector<HomgLineSeg2D>& l2) const;

  // Data Control--------------------------------------------------------------
  void set(LineSegSet* lines1, LineSegSet* lines2);
};

#endif // PairMatchSetLineSeg_h_
