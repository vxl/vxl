#ifndef _HMatrix2DComputeLinear_h
#define _HMatrix2DComputeLinear_h

//--------------------------------------------------------------
//
// .NAME HMatrix2DComputeLinear
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DComputeLinear.h
// .FILE HMatrix2DComputeLinear.cxx
//
// .SECTION Description:
//
// HMatrix2DComputeLinear contains a linear method to calculate
// the plane projectivity which relates four 2D point correspondences.
// @{ The returned $H$ is such that
// \[
//   H ~ [p_1 ~ p_2 ~ p_3 ~ p_4 ] \sim [p'_1 ~ p'_2 ~ p'_3 ~ p'_4 ]
// \]
// Where the $p_i$ are the homogeneous points in the first view, and the
// $p'_i$ their images. @}
//
// .SECTION Modifications:
//     200598 FSM added checks for degenerate or coincident points.
//

#include "HMatrix2DCompute.h"

class HMatrix2DComputeLinear : public HMatrix2DCompute {
//--------------------------------------------------------------------------------=
private:
  bool _allow_ideal_points;
//--------------------------------------------------------------------------------=
protected:
  bool compute_p(const PointArray&,
                 const PointArray&,
                 HMatrix2D* H);
//--------------------------------------------------------------------------------=
public:
  HMatrix2DComputeLinear(bool allow_ideal_points = false);
  int minimum_number_of_correspondences() const { return 4; }
};

#endif // _HMatrix2DComputeLinear_h
