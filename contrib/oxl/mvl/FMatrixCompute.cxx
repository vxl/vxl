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
#ifdef __GNUC__
#pragma implementation "FMatrixCompute.h"
#endif
//
// Class: FMatrixCompute
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 14 Sep 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrix.h>
#include <mvl/FMatrixCompute.h>
#include <mvl/PairSetCorner.h>

//---------------------------------------------------------------
//
// -- Construct an FMatrixCompute which points to the given list
// of 2D point matches.  The list is owned by the caller, and must
// remain in scope at least as long as the FMatrixCompute.
// 
FMatrixCompute::FMatrixCompute ()
{
}

//---------------------------------------------------------------
//
// -- Destructor.
//
FMatrixCompute::~FMatrixCompute()
{
}

// @{COMPUTATIONS@}

//-----------------------------------------------------------------------------
// -- Compute fundamental matrix using given matchlist.  This is implemented in terms
// of compute(FMatrix*).  This circular dependency means that only one of these two
// functions need be implemented for both to work.
//
// It also means that calling one of these on a base FMatrixCompute will blow
// your stack, but FMatrixCompute is abstract so that ``can't happen''.

bool FMatrixCompute::compute (PairMatchSetCorner& matches, FMatrix* F_out)
{
  PairSetCorner inliers(matches);
  return compute(inliers.points1, inliers.points2, F_out);
}

// -- Compute fundamental matrix using given matchlist and return an
// FMatrix object.  This is implemented in terms of compute(MatchList*, FMatrix*)

FMatrix FMatrixCompute::compute (PairMatchSetCorner& matches)
{
  FMatrix ret;
  compute(matches, &ret);
  return ret;
}

FMatrix FMatrixCompute::compute (vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2)
{
  FMatrix ret;
  compute(points1, points2, &ret);
  return ret;
}

bool FMatrixCompute::compute (vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2,
			      FMatrix* out)
{
  *out = compute(points1, points2);
  return true;
}
