//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vnl_matrix_fixed_pairwise_ops.h"
#endif
//
// Class: vnl_matrix_fixed_pairwise_ops
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 23 Apr 99
// Modifications:
//   990423 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_matrix_fixed.txx>

#if defined(WIN32) || defined(IUE_SUNPRO_CC)
// don't do anything.
#else
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 2,2,2);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 2,3,3);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 3,3,3);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 3,3,4);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 3,4,4);
VNL_MATRIX_FIXED_PAIR_INSTANTIATE(double, 4,4,4);
#endif
