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
#pragma implementation
#endif

//-----------------------------------------------------------------------------
//
// Class: FMatrixComputeLinear
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Jul 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_cassert.h>
#include <vcl/vcl_vector.h>
#include <vcl/vcl_iostream.h>

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_matops.h> // use vnl_matlab_print.h for pretty printing

#include <mvl/FMatrix.h>
#include <mvl/HomgMetric.h>

#include <mvl/PairMatchSetCorner.h>
#include "FMatrixComputeLinear.h"
#include "FDesignMatrix.h"
#include "HomgNorm2D.h"

FMatrixComputeLinear::FMatrixComputeLinear(bool precondition, bool rank2_truncate):
  _precondition(precondition),
  _rank2_truncate(rank2_truncate)
{
}

//-----------------------------------------------------------------------------
//
// - Compute a fundamental matrix for a set of point matches.
// 
// Return false if the calculation fails or there are fewer than eight point
// matches in the list.
//

bool FMatrixComputeLinear::compute (PairMatchSetCorner& matches, FMatrix *F)
{
  // Copy matching points from matchset.
  vcl_vector<HomgPoint2D> points1(matches.count()); 
  vcl_vector<HomgPoint2D> points2(matches.count());
  matches.extract_matches(points1, points2);
  return compute(points1, points2, F);
}

//-----------------------------------------------------------------------------
bool FMatrixComputeLinear::compute (vcl_vector<HomgPoint2D>& points1,
				    vcl_vector<HomgPoint2D>& points2, FMatrix *F)
{
  if (points1.size() < 8 || points2.size() < 8) {
    cerr << "FMatrixComputeLinear: Need at least 8 point pairs." << endl;
    cerr << "Number in each set: " << points1.size() << ", " << points2.size() << endl;
    return false;
  }

  if (_precondition) {
    // Condition points
    HomgNorm2D conditioned1(points1);
    HomgNorm2D conditioned2(points2);

    // Compute F with preconditioned points
    compute_preconditioned(conditioned1.get_normalized_points(),
			   conditioned2.get_normalized_points(),
			   F);

    // De-condition F
    *F = HomgMetric::homg_to_image_F(*F, &conditioned1, &conditioned2);
  } else
    compute_preconditioned(points1, points2, F);

  return true;
}

//-----------------------------------------------------------------------------
bool FMatrixComputeLinear::compute_preconditioned (vcl_vector<HomgPoint2D>& points1,
						   vcl_vector<HomgPoint2D>& points2,
						   FMatrix *F)
{
  // Create design matrix from conditioned points.
  FDesignMatrix design(points1, points2);
  
  // Normalize rows for better conditioning
  design.normalize_rows();
  
  // Extract vnl_svd<double> of design matrix
  vnl_svd<double> svd (design);
  
  // Reshape nullvector to 3x3
  F->set(vnl_double_3x3(svd.nullvector().data_block()));

  // Rank-truncate F
  if (_rank2_truncate)
    F->set_rank2_using_svd();

  return true;
}
