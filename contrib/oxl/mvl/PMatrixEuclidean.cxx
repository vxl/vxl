// This is oxl/mvl/PMatrixEuclidean.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//
// Class: PMatrixEuclidean
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 24 Feb 97
// Modifications:
//   970224 AWF Initial version.
//   000621 Peter Vanroose removed implementation for compiler-supplied methods
//
//-----------------------------------------------------------------------------

#include "PMatrixEuclidean.h"

bool PMatrixEuclidean::decompose_to_intrinsic_extrinsic_params (
  vnl_matrix<double> * /* camera_matrix */,
  vnl_matrix<double> * /* R */,
  vnl_vector<double> * /* t */) {
  return false; // FIXME: not yet implemented
};
