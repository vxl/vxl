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
#ifdef __GNUC__
#pragma implementation "TripleMatchSetLineSeg.h"
#endif
//
// Class: TripleMatchSetLineSeg
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 09 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_cassert.h>

#include <vcl/vcl_vector.h>
#include <mvl/HomgLineSeg2D.h>

#include <mvl/LineSegSet.h>

#include "PairMatchSetLineSeg.h"
#include "TripleMatchSetLineSeg.h"


TripleMatchSetLineSeg::TripleMatchSetLineSeg():
  TripleMatchSet(new PairMatchSetLineSeg, new PairMatchSetLineSeg)
{
}

TripleMatchSetLineSeg::TripleMatchSetLineSeg(LineSegSet& lines1,
					     LineSegSet& lines2,
					     LineSegSet& lines3):
  TripleMatchSet(new PairMatchSetLineSeg(lines1, lines2),
		 new PairMatchSetLineSeg(lines2, lines3))
{
}

TripleMatchSetLineSeg::TripleMatchSetLineSeg(const PairMatchSetLineSeg& match12,
					     const PairMatchSetLineSeg& match23):
  TripleMatchSet(new PairMatchSetLineSeg(*match12.get_linesegs1(), *match12.get_linesegs2()),
		 new PairMatchSetLineSeg(*match23.get_linesegs1(), *match23.get_linesegs2()))
{
  assert(match23.get_linesegs1() == match12.get_linesegs2());
  set_from_pairwise_matches(match12, match23);
}

void TripleMatchSetLineSeg::set(LineSegSet& lines1, LineSegSet& lines2, LineSegSet& lines3)
{
  TripleMatchSet::set(new PairMatchSetLineSeg(lines1, lines2), new PairMatchSetLineSeg(lines2, lines3));
}

void TripleMatchSetLineSeg::extract_matches(vcl_vector<HomgLineSeg2D> &linesegs1, vcl_vector<int> &indices1,
					    vcl_vector<HomgLineSeg2D> &linesegs2, vcl_vector<int> &indices2,
					    vcl_vector<HomgLineSeg2D> &linesegs3, vcl_vector<int> &indices3) const 
{
  const LineSegSet* _linesegs1 = get_linesegs1();
  const LineSegSet* _linesegs2 = get_linesegs2();
  const LineSegSet* _linesegs3 = get_linesegs3();

  int n = count();
  linesegs1.resize(n);
  linesegs2.resize(n);
  linesegs3.resize(n);
  
  indices1.resize(n);
  indices2.resize(n);
  indices3.resize(n);

  int i = 0;
  for(iterator p = begin(); p; ++p, ++i) {
    indices1[i] = p.get_i1();
    indices2[i] = p.get_i2();
    indices3[i] = p.get_i3();
    linesegs1[i] = _linesegs1->get_homg(indices1[i]);
    linesegs2[i] = _linesegs2->get_homg(indices2[i]);
    linesegs3[i] = _linesegs3->get_homg(indices3[i]);
  }
}
