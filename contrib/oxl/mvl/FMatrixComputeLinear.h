// This is oxl/mvl/FMatrixComputeLinear.h
#ifndef FMatrixComputeLinear_h_
#define FMatrixComputeLinear_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Hartley 8-point fundamental matrix fit
//
//    FMatrixComputeLinear is a subclass of FMatrixCompute, and implements
//    the ``8-point'' or linear least squares algorithm for estimation of
//    the fundamental matrix.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// \example examples/exampleFMatrixCompute.cxx
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 21 Aug 96
//
//-----------------------------------------------------------------------------

#include <mvl/FMatrixCompute.h>
#include <mvl/FMatrix.h>

class FMatrixComputeLinear : public FMatrixCompute
{
 public:
//: Initialize FMatrixComputeLinear object.
//  If precondition = false, points are not conditioned prior to computation.
// If rank2_truncate = false, the resulting solution is not forced to rank 2
// using the vnl_svd<double>.
  FMatrixComputeLinear(bool precondition = true, bool rank2_truncate = true);

  // Computations--------------------------------------------------------------

//: Compute a fundamental matrix for a set of point matches.
//
// Return false if the calculation fails or there are fewer than eight point
// matches in the list.
//
  bool compute(PairMatchSetCorner&, FMatrix* F);

//: Interface to above using arrays of HomgPoint2D.
//  Makes a PairMatchSetCorner, and then calls the compute method above.
  bool compute(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* F);

//: Interface to above using preconditioned points
  bool compute_preconditioned(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* F);

//: Return an FMatrix computed as above.
  FMatrix compute(PairMatchSetCorner& matches) { return FMatrixCompute::compute(matches); }
  FMatrix compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2) {
    return FMatrixCompute::compute(points1, points2);
  }

 private:
  bool _precondition;
  bool _rank2_truncate;
};

#endif // FMatrixComputeLinear_h_
