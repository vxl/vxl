#ifndef _PMatrixCompute6Point_h
#define _PMatrixCompute6Point_h

//--------------------------------------------------------------
//
// .NAME PMatrixCompute6Point
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/PMatrixCompute6Point.h
// .FILE PMatrixCompute6Point.cxx
//
// .SECTION Description:
// PMatrixCompute6Point contains a linear method to calculate the P matrix
// given the minimum  number of points necessary to get a unique solution
// upto scale i.e. six points.
//

#include <vcl/vcl_list.h>
#include <mvl/PMatrixCompute.h>

class PMatrixCompute6Point : public PMatrixCompute {
public:
  bool compute (PMatrix *t_matrix_ptr);
};

#endif // _PMatrixCompute6Point_h
