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
// - FMatrixCompute8Point    - linear least squares on 8 point matches
// - FMatrixComputeLMedSq    - Zhengyou Zhang's Least Medium of Squares estimation
// - FMatrixComputeMLESAC    - Phil Torr's Maximum Likelyhood estimation
// - FMatrixComputeRANSAC    - Phil Torr's Robust Sampling Concensus
//

#include <vcl_vector.h>
#include <mvl/FMatrix.h>
class HomgPoint2D;
class PairMatchSetCorner;

class FMatrixCompute
{
 public:
  FMatrixCompute();
  virtual ~FMatrixCompute();

  //: This is the virtual compute interface
  // Both functions are implemented in terms of each other,
  // so it suffices to implement exactly one of them in a derived class,
  // and implement the other one by calling this implementation.
  virtual bool compute (PairMatchSetCorner& matched_points, FMatrix* f_matrix_ptr);
  virtual bool compute (vcl_vector<HomgPoint2D>&, vcl_vector<HomgPoint2D>&, FMatrix* f_matrix_ptr);

  //: Compute fundamental matrix using given matchlist and return an FMatrix object.
  //  This is implemented in terms of compute(MatchList*, FMatrix*)
  inline FMatrix compute(PairMatchSetCorner& matched_points)
    { FMatrix* ret; compute(matched_points, ret); return *ret; }

  inline FMatrix compute(vcl_vector<HomgPoint2D>& pts1, vcl_vector<HomgPoint2D>& pts2)
    { FMatrix* ret; compute(pts1, pts2, ret); return *ret; }
};

#endif // _FMatrixCompute_h
