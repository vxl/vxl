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

//--------------------------------------------------------------
//
// Class : HMatrix2DCompute4Point
//
// Modifications : see HMatrix2DCompute4Point.h
//
//--------------------------------------------------------------

#include "HMatrix2DCompute4Point.h"

#include <vcl/vcl_vector.h>
#include <mvl/HMatrix2D.h>
#include <mvl/ProjectiveBasis2D.h>

//-----------------------------------------------------------------------------
//
// - Compute a plane-plane projectivity using linear least squares.
// Returns false if the calculation fails or there are fewer than four point
// matches in the list.
//


//-----------------------------------------------------------------------------
//
// -- @{ Compute a plane-plane projectivity using 4 point correspondences.
// Returns false if the calculation fails or there are fewer than four point
// matches in the list.
//
// The algorithm determines the transformation $H_i$ from each pointset to the
// canonical projective basis (see the {\bf ProjectiveBasis2D} class), and
// returns the combined transform $H = H_2^{-1} H_1$.
//
// @}

bool
HMatrix2DCompute4Point::compute_p(const vcl_vector<HomgPoint2D>& points1,
				  const vcl_vector<HomgPoint2D>& points2,
				  HMatrix2D *H)
{
  ProjectiveBasis2D basis1(points1);
  if ( basis1.collinear() ) return false;
  ProjectiveBasis2D basis2(points2);
  if ( basis2.collinear() ) return false;

  H->set(basis2.get_T().get_inverse().get_matrix() * basis1.get_T_matrix());
  return true;
}

