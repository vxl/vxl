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
#ifndef PairMatchSetCorner_h_
#define PairMatchSetCorner_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : PairMatchSetCorner
//
// .SECTION Description
//    PairMatchSetCorner is a subclass of PairMatchSet that stores matches
//    between corner features.
//
// .NAME        PairMatchSetCorner - Matches between corners.
// .LIBRARY     MViewBasics
// .HEADER	MultiView Package
// .INCLUDE     mvl/PairMatchSetCorner.h
// .FILE        PairMatchSetCorner.h
// .FILE        PairMatchSetCorner.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 09 Aug 96
//
// .SECTION Modifications:
//     <none yet>
//
//-----------------------------------------------------------------------------

//////////////#include <cool/decls.h>
#include <mvl/HomgPoint2D.h>
#include "PairMatchSet.h"

class HomgInterestPointSet;
class PairMatchSetCorner : public PairMatchSet {
public:
  // Constructors/Destructors--------------------------------------------------
  
  PairMatchSetCorner();
  // deprecated PairMatchSetCorner(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2);
  PairMatchSetCorner(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2);
  PairMatchSetCorner(const PairMatchSetCorner& that);
  PairMatchSetCorner& operator=(const PairMatchSetCorner& that);
 ~PairMatchSetCorner();

  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------
  void extract_matches(vcl_vector <HomgPoint2D>& points1, vcl_vector <int>& corner_index_1,
		       vcl_vector <HomgPoint2D>& points2, vcl_vector <int>& corner_index_2) const;

  void extract_matches(vcl_vector <HomgPoint2D>& points1, vcl_vector <HomgPoint2D>& points2) const;

  // -- Clear all matches and then set only those for which the corresponding
  // inliers flag is set.
  void set(const vcl_vector<bool>& inliers,
	   const vcl_vector<int>&  corner_index_1,
	   const vcl_vector<int>&  corner_index_2);

  // Data Control--------------------------------------------------------------
  void set(HomgInterestPointSet const* corners1, HomgInterestPointSet const* corners2);

// -- Return the set of corners within which the i1 indices point
  HomgInterestPointSet const* get_corners1() const { return _corners1; }
  // HomgInterestPointSet* get_corners1() { return _corners1; }
  
// -- Return the set of corners within which the i2 indices point
  HomgInterestPointSet const* get_corners2() const { return _corners2; }
  //   HomgInterestPointSet* get_corners2() { return _corners2; }
 
 
private:
  HomgInterestPointSet const* _corners1;
  HomgInterestPointSet const* _corners2;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS PairMatchSetCorner.

