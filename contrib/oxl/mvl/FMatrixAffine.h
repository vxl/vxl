#ifndef _FMatrixAffine_h
#define _FMatrixAffine_h
#ifdef __GNUC__
#pragma interface
#endif

//--------------------------------------------------------------
//
// .NAME FMatrixAffine - Affine fundamental matrix
// .LIBRARY MViewBasics
// .HEADER MultiView package
// .INCLUDE mvl/FMatrixAffine.h
// .FILE FMatrixAffine.cxx
//
// .SECTION Description:
// A class to hold a Fundamental Matrix of the affine form
// and to perform common operations e.g. generate epipolar lines,
// inherited from the class FMatrix.
//

#include <vnl/vnl_matrix.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/FMatrix.h>

class FMatrixAffine : public FMatrix {

  // PUBLIC INTERFACE-------------------------------------------------------
public:

  // Constructors/Initializers/Destructors----------------------------------
  FMatrixAffine();
  ~FMatrixAffine();

  // Data Access------------------------------------------------------------

  bool set (const double *f_matrix);
};

#endif // _FMatrixAffine_h
