// This is oxl/mvl/FMPlanarComputeNonLinear.h
#ifndef FMPlanarComputeNonLinear_h_
#define FMPlanarComputeNonLinear_h_
//:
// \file
// \brief Nonlinear least squares planar fundamental matrix fit
//
// FMPlanarComputeNonLinear fits a planar fundamental matrix to point matches
// by minimizing the Luong-Faugeras [ECCV '92] error function
// \f\[
// E = \sum_{i=1}^n d^2({\bf x}'_i, {\tt F} {\bf x}_i) + d^2({\bf x}_i, {\tt F}^\top {\bf x}'_i)
// \f\]
// Minimization currently uses vnl_levenberg_marquardt with finite-difference
// derivatives, and does not minimize a Huber function---all matches
// are assumed to be inliers.
//
// \author
//   Martin Armstrong, Oxford 21/11/96
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vgl/vgl_fwd.h>
#include <mvl/FMatrixCompute.h>

class ImageMetric;
class PairMatchSetCorner;
class FMatrixPlanar;

class FMPlanarComputeNonLinear : public FMatrixCompute
{
 protected:
  // Data Members--------------------------------------------------------------
  double outlier_distance_squared_;

  const ImageMetric* image_metric1_;
  const ImageMetric* image_metric2_;

 public:

  // Constructors/Destructors--------------------------------------------------
  FMPlanarComputeNonLinear(const ImageMetric*, const ImageMetric*,
                           double outlier_threshold = 0);

  // Computations--------------------------------------------------------------

  bool compute_planar(std::vector<vgl_homg_point_2d<double> >& points1,
                      std::vector<vgl_homg_point_2d<double> >& points2,
                      FMatrixPlanar& F);
  bool compute_planar(PairMatchSetCorner& matches, FMatrixPlanar* F);
  bool compute_planar(std::vector<HomgPoint2D>& points1,
                      std::vector<HomgPoint2D>& points2, FMatrixPlanar* F);

  // FMatrixCompute virtuals
  bool compute(std::vector<vgl_homg_point_2d<double> >& points1,
               std::vector<vgl_homg_point_2d<double> >& points2, FMatrix& F) override;
  bool compute(PairMatchSetCorner& matches, FMatrix* F) override;
  bool compute(std::vector<HomgPoint2D>& points1,
               std::vector<HomgPoint2D>& points2, FMatrix* F) override;
};

#endif // FMPlanarComputeNonLinear_h_
