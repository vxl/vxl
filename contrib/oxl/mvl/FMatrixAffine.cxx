// This is oxl/mvl/FMatrixAffine.cxx
//:
//  \file

#include "FMatrixAffine.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//--------------------------------------------------------------
//
//: Constructor
FMatrixAffine::FMatrixAffine () = default;

//--------------------------------------------------------------
//
//: Destructor
FMatrixAffine::~FMatrixAffine() = default;

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the vnl_matrix f_matrix.
// Only returns true if f_matrix contained a Fundamental
// matrix in the affine form, and not an approximation to one.
// Otherwise returns false and the matrix is not set.
// f_matrix must be 3x3.

bool FMatrixAffine::set(vnl_matrix<double> const& f_matrix)
{
  assert(f_matrix.rows() == 3 && f_matrix.columns() == 3);
  for (int row_index = 0; row_index < 3; row_index++)
  for (int col_index = 0; col_index < 3; col_index++)
  {
     f_matrix_.put(row_index, col_index, f_matrix(row_index,col_index));
    ft_matrix_.put(col_index, row_index, f_matrix(row_index,col_index));
  }
  return true;
}

//--------------------------------------------------------------
//
//: Set the fundamental matrix using the two-dimensional (C-storage) array f_matrix.
// Only returns true if f_matrix contained a Fundamental
// matrix in the affine form, and not an approximation to one.
// Otherwise returns false and the matrix is not set.
// f_matrix must be 3x3, i.e., must contain 9 elements.

bool FMatrixAffine::set(const double* f_matrix)
{
  for (int row_index = 0; row_index < 3; row_index++)
  for (int col_index = 0; col_index < 3; col_index++)
  {
     f_matrix_.put(row_index, col_index, *f_matrix);
    ft_matrix_.put(col_index, row_index, *f_matrix++);
  }
  return true;
}
