#ifndef FMatrixAffine_h_
#define FMatrixAffine_h_
#ifdef __GNUC__
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
