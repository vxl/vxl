// This is oxl/mvl/FMPlanarComputeNonLinear.cxx
//:
// \file

#include <iostream>
#include "FMPlanarComputeNonLinear.h"
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/PairSetCorner.h>
#include <mvl/FMatrixPlanar.h>
#include <mvl/FMPlanarNonLinFun.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor.
// The parameter outlier_threshold is not currently used, but
// may be in future if this is converted to a Huber function.
FMPlanarComputeNonLinear::FMPlanarComputeNonLinear(const ImageMetric* image_metric1,
                                                   const ImageMetric* image_metric2,
                                                   double outlier_threshold)
  : FMatrixCompute()
{
  image_metric1_ = image_metric1;
  image_metric2_ = image_metric2;
  outlier_distance_squared_ = outlier_threshold * outlier_threshold;
}

//: Compute from given PairMatchSetCorner
bool FMPlanarComputeNonLinear::compute_planar(PairMatchSetCorner& matches, FMatrixPlanar* F)
{
  PairSetCorner inliers(matches);
  return compute_planar(inliers.points1, inliers.points2, F);
}

//: Compute from given pair of std::vector<HomgPoint2D>
bool FMPlanarComputeNonLinear::compute_planar(std::vector<HomgPoint2D>& points1,
                                              std::vector<HomgPoint2D>& points2,
                                              FMatrixPlanar* F)
{
  std::cerr << "FMPlanarComputeNonLinear: Fitting planar-motion F matrix [e1]_x [l]_x [e2]_x\n";
  FMPlanarNonLinFun computor(image_metric1_, image_metric2_, outlier_distance_squared_, points1, points2);
  return computor.compute(F);
}

//: Compute from given pair of std::vector<vgl_homg_point_2d<double> >
bool FMPlanarComputeNonLinear::compute_planar(std::vector<vgl_homg_point_2d<double> >& points1,
                                              std::vector<vgl_homg_point_2d<double> >& points2,
                                              FMatrixPlanar& F)
{
  std::cerr << "FMPlanarComputeNonLinear: Fitting planar-motion F matrix [e1]_x [l]_x [e2]_x\n";
  FMPlanarNonLinFun computor(image_metric1_, image_metric2_, outlier_distance_squared_, points1, points2);
  return computor.compute(&F);
}

bool FMPlanarComputeNonLinear::compute(PairMatchSetCorner& matches, FMatrix* F)
{
  FMatrixPlanar fplanar;
  fplanar.init(*F);
  if (!compute_planar(matches, &fplanar))
    return false;

  // Slice Fplanar into F
  *F = fplanar;
  return true;
}

bool FMPlanarComputeNonLinear::compute(std::vector<HomgPoint2D>& points1,
                                       std::vector<HomgPoint2D>& points2,
                                       FMatrix* F)
{
  FMatrixPlanar fplanar;
  fplanar.init(*F);
  if (!compute_planar(points1, points2, &fplanar))
    return false;

  // Slice Fplanar into F
  *F = fplanar;
  return true;
}

bool FMPlanarComputeNonLinear::compute(std::vector<vgl_homg_point_2d<double> >& points1,
                                       std::vector<vgl_homg_point_2d<double> >& points2,
                                       FMatrix& F)
{
  FMatrixPlanar fplanar;
  fplanar.init(F);
  if (!compute_planar(points1, points2, fplanar))
    return false;

  // Slice Fplanar into F
  F = fplanar;
  return true;
}
