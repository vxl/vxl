#ifndef PairMatchSetLineSeg_h_
#define PairMatchSetLineSeg_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME    PairMatchSetLineSeg - Matches between homogeneous line segments
// .LIBRARY MViewBasics
// .HEADER  MultiView package
// .INCLUDE mvl/PairMatchSetLineSeg.h
// .FILE    PairMatchSetLineSeg.cxx
//
// .SECTION Description
//    PairMatchSetLineSeg holds matches between homogeneous
//    line segments.  This should be rewritten to use the original
//    IULine*, but this is efficient.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSet.h>

class LineSegSet;
class HomgLineSeg2D;

class PairMatchSetLineSeg : public PairMatchSet {
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

private:
  LineSegSet* _linesegs1;
  LineSegSet* _linesegs2;
};

#endif // PairMatchSetLineSeg_h_
