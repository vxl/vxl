// This is oxl/mvl/FMPlanarComputeNonLinear.h
#ifndef FMPlanarComputeNonLinear_h_
#define FMPlanarComputeNonLinear_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Nonlinear least squares planar fundamental matrix fit
//
//    FMPlanarComputeNonLinear fits a planar fundamental matrix to point matches
//    by minimizing the Luong-Faugeras [ECCV '92] error function
//    \f\[
//    E = \sum_{i=1}^n d^2({\bf x}'_i, {\tt F} {\bf x}_i) + d^2({\bf x}_i, {\tt F}^\top {\bf x}'_i)
//    \f\]
//    Minimization currently uses vnl_levenberg_marquardt with finite-difference
//    derivatives, and does not minimize a Huber function---all matches
//    are assumed to be inliers.
//
// \author
//     Martin Armstrong, Oxford 21/11/96
//
//-----------------------------------------------------------------------------

#include <mvl/HomgMetric.h>
#include <mvl/FMatrixCompute.h>

class ImageMetric;
class PairMatchSetCorner;
class FMatrixPlanar;

class FMPlanarComputeNonLinear : public FMatrixCompute
{
 public:

  // Constructors/Destructors--------------------------------------------------
  FMPlanarComputeNonLinear(const ImageMetric*, const ImageMetric*, double outlier_threshold = 0);

  // Operations----------------------------------------------------------------

  // Computations--------------------------------------------------------------

  bool compute_planar(PairMatchSetCorner& matches, FMatrixPlanar* F);
  bool compute_planar(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, FMatrixPlanar* F);

  // FMatrixCompute virtuals
  bool compute(PairMatchSetCorner& matches, FMatrix* F);
  bool compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2, FMatrix* F);

 protected:
  // Data Members--------------------------------------------------------------
  double outlier_distance_squared_;

  const ImageMetric* image_metric1_;
  const ImageMetric* image_metric2_;
};

#endif // FMPlanarComputeNonLinear_h_
