#ifndef FMatrixCompute7Point_h_
#define FMatrixCompute7Point_h_
//:
// \file
// \brief SVD 7 Point F Matrix Fit
//
//    FMatrixCompute7Point implements the `7-point' estimation
//    of the fundamental matrix.
//
//    Points are preconditioned as described in [Hartley, ``In defence of
//    the 8-point algorithm'', ICCV95], and the resulting F matrix is rank-2
//    truncated.  The conditioning and truncation are optional and may be
//    omitted.
//
//    The root finder is adapted from Phil Torr's code for the FMatrix
//    which was in turn adapted from numerical recipes in C.
//
//    Note: As with any nonrobust algorithm, mismatches in the input data
//    may severely effect the result.
//
// \author
//     David N. McKinnon, UQ I.R.I.S., 25 Nov 2000
//
//-----------------------------------------------------------------------------
#include <mvl/FMatrix.h>
#include <vcl_vector.h>
#include <mvl/PairMatchSetCorner.h>

class FMatrixCompute7Point {
public:
  //: Initialize FMatrixCompute7Point object.  If precondition = false,
  // points are not conditioned prior to computation.
  // If rank2_truncate = false, the resulting solution is not forced to rank 2
  // using the vnl_svd<double>.
  FMatrixCompute7Point(bool precondition = true, bool rank2_truncate = true);

  // Computations--------------------------------------------------------------

  //: Compute a fundamental matrix for a set of point matches.
  // Return false if the calculation fails or there are fewer than seven point
  // matches in the list.
  //
  bool compute(PairMatchSetCorner&, vcl_vector<FMatrix*>&);

  //: Interface to above using arrays of HomgPoint2D.  Makes a PairMatchSetCorner,
  // and then calls the compute method above.
  bool compute(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, vcl_vector<FMatrix*>&);

  // - Interface to above using preconditioned points
  bool compute_preconditioned(vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, vcl_vector<FMatrix*>&);

protected:
  bool GetCoef(FMatrix* F1, FMatrix* F2, vnl_vector<double> *a);
  int solve_quadratic(double a, double b, double c, double *x1, double *x2);
  double cbrt(double x);
  int solve_cubic(double a, double b, double c, double d, double *x1, double *x2, double *x3);

  bool precondition_;
  bool rank2_truncate_;
};

#endif // FMatrixCompute7Point_h_
