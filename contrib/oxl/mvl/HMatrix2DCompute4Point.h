#ifndef _HMatrix2DCompute4Point_h
#define _HMatrix2DCompute4Point_h

//--------------------------------------------------------------
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
//      08-02-98 FSM obsoleted bool compute(HMatrix2D *)
// \endverbatim

#include "HMatrix2DCompute.h"

class HMatrix2DCompute4Point : public HMatrix2DCompute {
public:
  int minimum_number_of_correspondences() const override { return 4; }

protected:
  bool compute_p(PointArray const &,
                 PointArray const &,
                 HMatrix2D *) override;
};

#endif // _HMatrix2DCompute4Point_h
