// This is oxl/mvl/FMatrixAffine.h
#ifndef FMatrixAffine_h_
#define FMatrixAffine_h_

//--------------------------------------------------------------
//:
// \file
// \brief Affine fundamental matrix
//
// A class to hold a Fundamental Matrix of the affine form
// and to perform common operations e.g. generate epipolar lines,
// inherited from the class FMatrix.
//

#include <mvl/FMatrix.h>

class FMatrixAffine : public FMatrix
{
 public:

  FMatrixAffine();
  ~FMatrixAffine() override;

  bool set (const double *f_matrix) override;
  bool set (const vnl_matrix<double>& f_matrix);
};

#endif // FMatrixAffine_h_
