// This is oxl/mvl/FMatrixComputeLinear.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

//-----------------------------------------------------------------------------
//
// .NAME FMatrixComputeLinear
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Jul 96
//
//-----------------------------------------------------------------------------

#include "FMatrixComputeLinear.h"

#include <vcl_vector.h>
#include <vcl_iostream.h>

#include <vnl/algo/vnl_svd.h>

#include <mvl/HomgMetric.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/FDesignMatrix.h>
#include <mvl/HomgNorm2D.h>

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
    vcl_cerr << "FMatrixComputeLinear: Need at least 8 point pairs.\n";
    vcl_cerr << "Number in each set: " << points1.size() << ", " << points2.size() << vcl_endl;
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
