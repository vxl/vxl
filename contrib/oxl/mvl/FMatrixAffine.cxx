// This is oxl/mvl/FMatrixAffine.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "FMatrixAffine.h"

//--------------------------------------------------------------
//
//: Constructor
FMatrixAffine::FMatrixAffine ()
{
}

//--------------------------------------------------------------
//
//: Destructor
FMatrixAffine::~FMatrixAffine()
{
}

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the two-dimensional (C-storage) array f_matrix.
// Only returns true if f_matrix contained a Fundamental
// matrix in the affine form, and not an approximation to one.
// Otherwise returns false and the matrix is not set.
// f_matrix must be 3x3, i.e., must contain 9 elements.

bool FMatrixAffine::set (const double* f_matrix)
{
  for (int row_index = 0; row_index < 3; row_index++)
  for (int col_index = 0; col_index < 3; col_index++)
  {
    f_matrix_. put (row_index, col_index, *f_matrix);
    ft_matrix_. put (col_index, row_index, *f_matrix++);
  }

  return true;
}
