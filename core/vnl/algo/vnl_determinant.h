// This is core/vnl/algo/vnl_determinant.h
#ifndef vnl_algo_determinant_h_
#define vnl_algo_determinant_h_
//:
// \file
// \brief calculates the determinant of a matrix
// \author fsm
//
//  Evaluation of determinants of any size. For small
//  matrices, will use the direct routines (no netlib)
//  but for larger matrices, a matrix decomposition
//  such as SVD or QR will be used.
//
// \verbatim
//  Modifications
//   dac (Manchester) 26/03/2001: tidied up documentation
//   Feb.2002 - Peter Vanroose - brief doxygen comment placed on single line
//   Sep.2003 - Peter Vanroose - specialisation for int added
// \endverbatim

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vcl_cassert.h>

//: direct evaluation for 2x2 matrix
template <class T> T vnl_determinant(T const *row0,
                                     T const *row1);

//: direct evaluation for 3x3 matrix
template <class T> T vnl_determinant(T const *row0,
                                     T const *row1,
                                     T const *row2);

//: direct evaluation for 4x4 matrix
template <class T> T vnl_determinant(T const *row0,
                                     T const *row1,
                                     T const *row2,
                                     T const *row3);

//: evaluation using direct methods for sizes of 2x2, 3x3, and 4x4 or qr decomposition for other matrices.
template <class T>
T vnl_determinant(vnl_matrix<T> const &M, bool balance = false);

//: convenience overload
// See other vnl_determinant.
template <class T, unsigned m, unsigned n>
inline T vnl_determinant(vnl_matrix_fixed<T,m,n> const &M, bool balance = false)
{
  return vnl_determinant( M.as_ref(), balance );
}

VCL_DEFINE_SPECIALIZATION
inline int vnl_determinant(vnl_matrix<int> const &M, bool balance)
{
  unsigned n = M.rows();
  assert(M.cols() == n);

  switch (n) {
   case 1: return M[0][0];
   case 2: return vnl_determinant(M[0], M[1]);
   case 3: return vnl_determinant(M[0], M[1], M[2]);
   case 4: return vnl_determinant(M[0], M[1], M[2], M[3]);
   default:
    vnl_matrix<double> m(n,n);
    for (unsigned int i=0; i<n; ++i)
      for (unsigned int j=0; j<n; ++j)
        m[i][j]=double(M[i][j]);
    return int(0.5+vnl_determinant(m, balance)); // round to nearest integer
  }
}

#define VNL_DETERMINANT_INSTANTIATE(T) \
extern "you must include vnl/algo/vnl_determinant.txx first"

#endif // vnl_algo_determinant_h_
