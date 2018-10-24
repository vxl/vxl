#ifndef _HMatrix2DComputeLinear_h
#define _HMatrix2DComputeLinear_h
//:
// \file
//
// HMatrix2DComputeLinear contains a linear method to calculate
// the plane projectivity which relates four 2D point correspondences.
// The returned $H$ is such that
// $H ~ [p_1 ~ p_2 ~ p_3 ~ p_4 ] \sim [p'_1 ~ p'_2 ~ p'_3 ~ p'_4 ]$
// where the $p_i$ are the homogeneous points in the first view, and the
// $p'_i$ their images.
//
// \verbatim
// Modifications:
//     200598 FSM added checks for degenerate or coincident points.
// \endverbatim

#include "HMatrix2DCompute.h"

class HMatrix2DComputeLinear : public HMatrix2DCompute
{
  bool allow_ideal_points_;

 protected:
  bool compute_p(PointArray const&,
                 PointArray const&,
                 HMatrix2D* H) override;

 public:
  HMatrix2DComputeLinear(bool allow_ideal_points = false);
  int minimum_number_of_correspondences() const override { return 4; }
};

#endif // _HMatrix2DComputeLinear_h
