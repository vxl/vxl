// This is oxl/mvl/FMatrixAffine.h
#ifndef FMatrixAffine_h_
#define FMatrixAffine_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//--------------------------------------------------------------
//:
// \file
// \brief Affine fundamental matrix
//
// A class to hold a Fundamental Matrix of the affine form
// and to perform common operations e.g. generate epipolar lines,
// inherited from the class FMatrix.
//

#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

class FMatrixAffine : public FMatrix
{
 public:

  FMatrixAffine();
  ~FMatrixAffine();

  bool set (const double *f_matrix);
};

#endif // FMatrixAffine_h_
