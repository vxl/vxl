#ifndef _PMatrixEuclidean_h
#define _PMatrixEuclidean_h
#ifdef __GNUC__
#pragma interface
#endif

//-*- c++ -*-------------------------------------------------------------------
//
// .NAME PMatrixEuclidean - 3x4 projection matrix
// .HEADER MultiView package
// .LIBRARY MViewBasics
// .INCLUDE mvl/PMatrixEuclidean.h
// .FILE PMatrixEuclidean.cxx
//
// .SECTION Description:
// A class to hold a calibrated perspective projection matrix,
// also called the perspective camera, and use it to
// perform common operations e.g. projecting point in 3D space to
// its image on the image plane
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/PMatrix.h>

class PMatrixEuclidean : public PMatrix {

  // PUBLIC INTERFACE----------------------------------------------------------
public:

  // Constructors/Initializers/Destructors-------------------------------------
  PMatrixEuclidean(void) {}
  ~PMatrixEuclidean(void) {}

  // Computations--------------------------------------------------------------

  bool decompose_to_intrinsic_extrinsic_params (vnl_matrix<double> *camera_matrix,
                                                vnl_matrix<double> *R,
                                                vnl_vector<double> *t);
};

#endif // _PMatrixEuclidean_h
