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
#ifndef PairMatchSetMulti_h_
#define PairMatchSetMulti_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : PairMatchSetMulti
//
// .SECTION Description
//    PairMatchSetMulti is a list of tuples (i1, i2) which allows
//    efficient O(log n) indexing by I1, and O(n) by i2.
//
// .NAME        PairMatchSetMulti - Multimap of ints.
// .LIBRARY MViewBasics
// .HEADER	MultiView Package
// .INCLUDE mvl/PairMatchSetMulti.h
// .FILE        PairMatchSetMulti.h
// .FILE        PairMatchSetMulti.C
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 16 Sep 96
//
//-----------------------------------------------------------------------------

class PairMatchSetMulti {
public:
  // Constructors/Destructors--------------------------------------------------
  
  PairMatchSetMulti();
  PairMatchSetMulti(const PairMatchSetMulti& that);
 ~PairMatchSetMulti();

  PairMatchSetMulti& operator=(const PairMatchSetMulti& that);

  // Operations----------------------------------------------------------------
  void add_match(int i1, int i2);
  
  // Computations--------------------------------------------------------------

  // Data Access---------------------------------------------------------------

  // Data Control--------------------------------------------------------------

protected:
  // Data Members--------------------------------------------------------------
  // multimap<int, int>

private:
  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS PairMatchSetMulti.

