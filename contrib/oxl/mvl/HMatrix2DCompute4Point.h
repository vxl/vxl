#ifndef _HMatrix2DCompute4Point_h
#define _HMatrix2DCompute4Point_h

//--------------------------------------------------------------
//
// .NAME HMatrix2DCompute4Point
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DCompute4Point.h
// .FILE HMatrix2DCompute4Point.cxx
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
//      08-02-98 FSM obsoleted bool compute(HMatrix2D *)
//

#include "HMatrix2DCompute.h"

class HMatrix2DCompute4Point : public HMatrix2DCompute {
//--------------------------------------------------------------------------------
protected:
  bool compute_p(const PointArray &,
		 const PointArray &,
		 HMatrix2D *);
};

#endif // _HMatrix2DCompute4Point_h
