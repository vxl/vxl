#ifndef vnl_algo_determinant_h_
#define vnl_algo_determinant_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \brief calculates the determinant of a matrix
// \author fsm@robots.ox.ac.uk
//
//  Evaluation of determinants of any size. For small
//  matrices, will use the direct routines (no netlib)
//  but for larger matrices, a matrix decomposition
//  such as SVD or QR will be used.
//
// \verbatim
// Modifications
//  dac (Manchester) 26/03/2001: tidied up documentation
// \endverbatim

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_determinant.h> // <-- not this file; different directory.

template <class T>
T vnl_determinant(T const *const *rows, int size, bool balance = false);

template <class T>
T vnl_determinant(vnl_matrix<T> const &M, bool balance = false);

#define VNL_DETERMINANT_INSTANTIATE(T) \
extern "you must include vnl/algo/vnl_determinant.txx first"

#endif // vnl_algo_determinant_h_
