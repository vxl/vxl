// This is oxl/mvl/PMatrixEuclidean.h
#ifndef PMatrixEuclidean_h_
#define PMatrixEuclidean_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief 3x4 projection matrix
//
// A class to hold a calibrated perspective projection matrix,
// also called the perspective camera, and use it to
// perform common operations e.g. projecting point in 3D space to
// its image on the image plane
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PMatrix.h>

class PMatrixEuclidean : public PMatrix
{
 public:
  // Constructors/Initializers/Destructors-------------------------------------
  PMatrixEuclidean(void) {}
  ~PMatrixEuclidean(void) {}

  // Computations--------------------------------------------------------------

  bool decompose_to_intrinsic_extrinsic_params (vnl_matrix<double> *camera_matrix,
                                                vnl_matrix<double> *R,
                                                vnl_vector<double> *t);
};

#endif // PMatrixEuclidean_h_
