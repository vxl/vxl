// This is oxl/mvl/PMatrixAffine.h
#ifndef PMatrixAffine_h_
#define PMatrixAffine_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//--------------------------------------------------------------
//:
// \file
// \brief Affine 3x4 projection matrix
//
// A class to hold an affine camera, a constrained form of the
// perspective projection matrix, and use it to
// perform common operations e.g. projecting point in 3d space to
// its image on the image plane
//

#include <mvl/PMatrix.h>

class PMatrixAffine : public PMatrix
{
 public:
  PMatrixAffine(void) {}
  ~PMatrixAffine(void) {}
};

#endif // PMatrixAffine_h_
