//--------------------------------------------------------------
//
// .NAME PMatrixCompute
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixCompute.h
// .FILE PMatrixCompute.cxx
//
// .SECTION Description:
// Base class of classes to generate a perspective projection matrix from
// a set of 3D-to-2D point correspondences.
//

#ifndef _PMatrixCompute_h
#define _PMatrixCompute_h

#include <vcl_list.h>

class PMatrix;
class HomgMatchPoint3D2D;

class PMatrixCompute {
public:
  // Constructors/Initializers/Destructors---------------------------------
  PMatrixCompute();
  virtual ~PMatrixCompute();

  // Data Control----------------------------------------------------------
  virtual bool compute (PMatrix *p_matrix_ptr);
};

#endif // _PMatrixCompute_h
