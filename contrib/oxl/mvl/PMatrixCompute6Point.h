#ifndef PMatrixCompute6Point_h_
#define PMatrixCompute6Point_h_
//:
// \file
//
// PMatrixCompute6Point contains a linear method to calculate the P matrix
// given the minimum  number of points necessary to get a unique solution
// upto scale i.e. six points.

#include <mvl/PMatrixCompute.h>

class PMatrixCompute6Point : public PMatrixCompute {
public:
  bool compute (PMatrix *t_matrix_ptr);
};

#endif // PMatrixCompute6Point_h_
