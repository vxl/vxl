#include "HMatrix2DCompute4Point.h"

#include <vcl_vector.h>
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
//: @{ Compute a plane-plane projectivity using 4 point correspondences.
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

