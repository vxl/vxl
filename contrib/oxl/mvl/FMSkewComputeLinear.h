// This is oxl/mvl/FMSkewComputeLinear.h
#ifndef FMSkewComputeLinear_h_
#define FMSkewComputeLinear_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Linear fit for skew F matrix
//
//    FMSkewComputeLinear is a subclass of FMatrixCompute, and implements
//    the linear least squares algorithm for estimation of
//    the fundamental matrix which has the skew form of FMatrixSkew.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95],but the conditioning is optional and may be
//    omitted. The same conditioning is applied to both sets of points so that
//    the skew form of the matrix is maintained.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// \author
//     Martin Armstrong, Oxford RRG, 20 Nov 96
//
// \verbatim
// Modifications:
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixCompute.h>

class FMatrixSkew;

class FMSkewComputeLinear : public FMatrixCompute
{
 public:
  //: Initialize FMSkewComputeLinear object.
  //  If precondition = false, points are not conditioned prior to computation.
  FMSkewComputeLinear(bool precondition = true);

  // Computations--------------------------------------------------------------

  //: Compute a fundamental matrix for arrays of HomgPoint2D.
  // Return false if the calculation fails or there are fewer than eight point
  // matches in the list.
  bool compute(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrixSkew* F);

 private:
  bool _precondition;
  bool compute_preconditioned(vcl_vector<HomgPoint2D>& points1,
                              vcl_vector<HomgPoint2D>& points2,
                              FMatrixSkew *F);
};

#endif // FMSkewComputeLinear_h_
