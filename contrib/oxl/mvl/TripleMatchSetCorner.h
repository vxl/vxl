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
#ifndef TripleMatchSetCorner_h_
#define TripleMatchSetCorner_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : TripleMatchSetCorner
//
// .SECTION Description
//    TripleMatchSetCorner is a TripleMatchSet that holds matches
//    between corners in three HomgInterestPointSets.
//
// .NAME        TripleMatchSetCorner - Three-view corner matches
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/TripleMatchSetCorner.h
// .FILE        TripleMatchSetCorner.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// .SECTION Modifications:
//     260297 AWF Added documentation
//
//-----------------------------------------------------------------------------

#include <mvl/PairMatchSetCorner.h>
#include "TripleMatchSet.h"

class TripleMatchSetCorner : public TripleMatchSet {
public:
  
  TripleMatchSetCorner();
  TripleMatchSetCorner(HomgInterestPointSet const* corners1,
		       HomgInterestPointSet const* corners2,
		       HomgInterestPointSet const* corners3);

  TripleMatchSetCorner(const TripleMatchSetCorner& that);
  TripleMatchSetCorner& operator=(const TripleMatchSetCorner& that);
  
  // -- Copy triplet matches out of two pairwise match sets.
  TripleMatchSetCorner(const PairMatchSetCorner& matches12, const PairMatchSetCorner& matches23);

  // -- Copy inliers to three arrays, and record the original indices. 
  void extract_matches(vcl_vector <HomgPoint2D>& points1, vcl_vector <int>& corner_index_1,
		       vcl_vector <HomgPoint2D>& points2, vcl_vector <int>& corner_index_2,
		       vcl_vector <HomgPoint2D>& points3, vcl_vector <int>& corner_index_3) const;

  // -- Copy inliers to three arrays
  void extract_matches(vcl_vector <HomgPoint2D>& points1,
		       vcl_vector <HomgPoint2D>& points2,
		       vcl_vector <HomgPoint2D>& points3) const;

  PairMatchSetCorner* get_matches12() { return (PairMatchSetCorner*)_match12; }
  PairMatchSetCorner* get_matches23() { return (PairMatchSetCorner*)_match23; }
  PairMatchSetCorner const* get_matches12() const { return (PairMatchSetCorner const*)_match12; }
  PairMatchSetCorner const* get_matches23() const { return (PairMatchSetCorner const*)_match23; }

  //HomgInterestPointSet* get_corners1() { return get_matches12()->get_corners1(); }
  //HomgInterestPointSet* get_corners2() { return get_matches12()->get_corners2(); }
  //HomgInterestPointSet* get_corners3() { return get_matches23()->get_corners2(); }
  HomgInterestPointSet const* get_corners1() const { return get_matches12()->get_corners1(); }
  HomgInterestPointSet const* get_corners2() const { return get_matches12()->get_corners2(); }
  HomgInterestPointSet const* get_corners3() const { return get_matches23()->get_corners2(); }
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS TripleMatchSetCorner.
