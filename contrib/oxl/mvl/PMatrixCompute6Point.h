#ifndef PMatrixCompute6Point_h_
#define PMatrixCompute6Point_h_
//:
// \file
//
//  PMatrixCompute6Point is actually identical to PMatrixComputeLinear,
//  a ``6-point'' linear least squares algorithm for estimation of
//  the projection matrix.
//
//  It calculates the P-matrix given the minimum number of points necessary
//  to get a unique solution upto scale viz. six points.
//

#include <mvl/PMatrixComputeLinear.h>

typedef PMatrixComputeLinear PMatrixCompute6Point;

#endif // PMatrixCompute6Point_h_
