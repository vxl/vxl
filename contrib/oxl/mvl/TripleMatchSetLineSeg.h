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
#ifndef TripleMatchSetLineSeg_h_
#define TripleMatchSetLineSeg_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : TripleMatchSetLineSeg
//
// .SECTION Description
//    TripleMatchSetLineSeg is a TripleMatchSet that holds matches
//    between corners in three LineSegSets.
//
// .NAME        TripleMatchSetLineSeg - Three-view line matches
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/TripleMatchSetLineSeg.h
// .FILE        TripleMatchSetLineSeg.h
// .FILE        TripleMatchSetLineSeg.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
//-----------------------------------------------------------------------------

//////////////#include <cool/decls.h>
#include "PairMatchSetLineSeg.h"
#include "TripleMatchSet.h"

class LineSegSet;
class PairMatchSetLineSeg;
class HomgLineSeg2D;
  
class TripleMatchSetLineSeg : public TripleMatchSet {
public:
  TripleMatchSetLineSeg();
  TripleMatchSetLineSeg(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3);
  TripleMatchSetLineSeg(const PairMatchSetLineSeg&, const PairMatchSetLineSeg&);

  void set(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3);

  PairMatchSetLineSeg* get_matches12() { return (PairMatchSetLineSeg*)_match12; }
  PairMatchSetLineSeg* get_matches23() { return (PairMatchSetLineSeg*)_match23; }

  LineSegSet* get_linesegs1() { return get_matches12()->get_linesegs1(); }
  LineSegSet* get_linesegs2() { return get_matches12()->get_linesegs2(); }
  LineSegSet* get_linesegs3() { return get_matches23()->get_linesegs2(); }

  PairMatchSetLineSeg const* get_matches12() const { return (PairMatchSetLineSeg const*)_match12; }
  PairMatchSetLineSeg const* get_matches23() const { return (PairMatchSetLineSeg const*)_match23; }

  LineSegSet const* get_linesegs1() const { return get_matches12()->get_linesegs1(); }
  LineSegSet const* get_linesegs2() const { return get_matches12()->get_linesegs2(); }
  LineSegSet const* get_linesegs3() const { return get_matches23()->get_linesegs2(); }

  // -- Copy inliers to three arrays, and record the original indices. 
  void extract_matches(vcl_vector <HomgLineSeg2D>& linesegs1, vcl_vector <int>& lineseg_index_1,
		       vcl_vector <HomgLineSeg2D>& linesegs2, vcl_vector <int>& lineseg_index_2,
		       vcl_vector <HomgLineSeg2D>& linesegs3, vcl_vector <int>& lineseg_index_3) const;

  // -- Copy inliers to three arrays
  void extract_matches(vcl_vector <HomgLineSeg2D>& linesegs1,
		       vcl_vector <HomgLineSeg2D>& linesegs2,
		       vcl_vector <HomgLineSeg2D>& linesegs3) const;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS TripleMatchSetLineSeg.

