#ifndef _PMatrixAffine_h
#define _PMatrixAffine_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME PMatrixAffine - Affine 3x4 projection matrix
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixAffine.h
// .FILE PMatrixAffine.cxx
//
// .SECTION Description:
// A class to hold an affine camera, a constrained form of the
// perspective projection matrix, and use it to
// perform common operations e.g. projecting point in 3d space to
// its image on the image plane
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/PMatrix.h>

class PMatrixAffine : public PMatrix {

  // PUBLIC INTERFACE--------------------------------------------------------

public:

  // Constructors/Initializers/Destructors-----------------------------------

  PMatrixAffine(void);
  ~PMatrixAffine(void);
};

#endif // _PMatrixAffine_h
