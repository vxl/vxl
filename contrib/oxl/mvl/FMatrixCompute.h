#ifndef _FMatrixCompute_h
#define _FMatrixCompute_h
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME FMatrixCompute - Base class for fundamental matrix fitters
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/FMatrixCompute.h
// .FILE FMatrixCompute.cxx
//
// .SECTION Description:
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
// * FMatrixComputeLinear	- Hartley's normalized linear method
// * FMatrixComputeRANSAC	- Oxford robust RANSAC computation
// * FMatrixComputeNonLinear	- Oxford nonlinear optimization
// * FMatrixCompute7Point	- Oxford implementation of 7-point algorithm
//

#include <vcl/vcl_vector.h>

#if defined(VCL_GCC_27)
# include <mvl/FMatrix.h>
# include <mvl/HomgPoint2D.h>
# include <mvl/PairMatchSetCorner.h>
#else
class FMatrix;
class HomgPoint2D;
class PairMatchSetCorner;
#endif

class FMatrixCompute {
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
