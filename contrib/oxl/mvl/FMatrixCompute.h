// This is oxl/mvl/FMatrixCompute.h
#ifndef _FMatrixCompute_h
#define _FMatrixCompute_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base class for fundamental matrix fitters
//
// FMatrixCompute defines the abstract interface to an object which can compute
// a fundamental matrix.  The method compute should be overloaded by
// implementation subclasses.
//
// The input points are supplied in a class PairMatchSetCorner, which may be
// updated by any robust methods to reflect the new set of inliers.
//
// Currently implemented subclasses:
//
// - FMatrixComputeLinear    - Hartley's normalized linear method
// - FMatrixComputeRANSAC    - Oxford robust RANSAC computation
// - FMatrixComputeNonLinear - Oxford nonlinear optimization
// - FMatrixCompute7Point    - Oxford implementation of 7-point algorithm
//

#include <vcl_vector.h>
class FMatrix;
class HomgPoint2D;
class PairMatchSetCorner;

class FMatrixCompute
{
 public:
  // Constructors/Initializers/Destructors----------------------------------
  FMatrixCompute();
  virtual ~FMatrixCompute();

  // Compute interface------------------------------------------------------
  virtual bool compute (PairMatchSetCorner& matched_points, FMatrix* f_matrix_ptr);
  virtual bool compute (vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* f_matrix_ptr);
  virtual FMatrix compute (PairMatchSetCorner& matched_points);
  virtual FMatrix compute (vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&);
};

#endif // _FMatrixCompute_h
