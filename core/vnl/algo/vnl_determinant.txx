#ifndef vnl_algo_determinant_txx_
#define vnl_algo_determinant_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_determinant.h"

#include <vcl_cassert.h>
#include <vnl/algo/vnl_qr.h>


template <class T>
T vnl_determinant(T const *row0, T const *row1) {
  return row0[0]*row1[1] - row0[1]*row1[0];
}

template <class T>
T vnl_determinant(T const *row0, T const *row1, T const *row2) {
  return // the extra '+' makes it work nicely with emacs indentation.
    + row0[0]*row1[1]*row2[2]
    - row0[0]*row2[1]*row1[2]
    - row1[0]*row0[1]*row2[2]
    + row1[0]*row2[1]*row0[2]
    + row2[0]*row0[1]*row1[2]
    - row2[0]*row1[1]*row0[2];
}

template <class T>
T vnl_determinant(T const *row0, T const *row1, T const *row2, T const *row3) {
  return
    + row0[0]*row1[1]*row2[2]*row3[3]
    - row0[0]*row1[1]*row3[2]*row2[3]
    - row0[0]*row2[1]*row1[2]*row3[3]
    + row0[0]*row2[1]*row3[2]*row1[3]
    + row0[0]*row3[1]*row1[2]*row2[3]
    - row0[0]*row3[1]*row2[2]*row1[3]
    - row1[0]*row0[1]*row2[2]*row3[3]
    + row1[0]*row0[1]*row3[2]*row2[3]
    + row1[0]*row2[1]*row0[2]*row3[3]
    - row1[0]*row2[1]*row3[2]*row0[3]
    - row1[0]*row3[1]*row0[2]*row2[3]
    + row1[0]*row3[1]*row2[2]*row0[3]
    + row2[0]*row0[1]*row1[2]*row3[3]
    - row2[0]*row0[1]*row3[2]*row1[3]
    - row2[0]*row1[1]*row0[2]*row3[3]
    + row2[0]*row1[1]*row3[2]*row0[3]
    + row2[0]*row3[1]*row0[2]*row1[3]
    - row2[0]*row3[1]*row1[2]*row0[3]
    - row3[0]*row0[1]*row1[2]*row2[3]
    + row3[0]*row0[1]*row2[2]*row1[3]
    + row3[0]*row1[1]*row0[2]*row2[3]
    - row3[0]*row1[1]*row2[2]*row0[3]
    - row3[0]*row2[1]*row0[2]*row1[3]
    + row3[0]*row2[1]*row1[2]*row0[3];
}

//--------------------------------------------------------------------------------



template <class T>
T vnl_determinant(T const *const *rows, int size, bool balance)
{
  vnl_matrix<T> tmp(size, size);
  for (int i=0; i<size; ++i)
    tmp.set_row(i, rows[i]);
  return vnl_determinant(tmp, balance);
}

template <class T>
T vnl_determinant(vnl_matrix<T> const &M, bool balance)
{
  unsigned n = M.rows();
  assert(M.cols() == n);
  
  switch (n) {
  case 1: return M[0][0];
  case 2: return vnl_determinant(M[0], M[1]);
  case 3: return vnl_determinant(M[0], M[1], M[2]);
  case 4: return vnl_determinant(M[0], M[1], M[2], M[3]);
  default:
    if (balance) {
      vnl_matrix<T> tmp(M);
      T scalings(1);
      for (int t=0; t<5; ++t) {
        for (int i=0; i<n; ++i) {
          T rn = tmp.get_row(i).two_norm();
          scalings *= rn;
          tmp.scale_row(i, T(1)/rn);
        }
        for (int i=0; i<n; ++i) {
          T rn = tmp.get_row(i).two_norm();
          scalings *= rn;
          tmp.scale_column(i, T(1)/rn);
        }
      }
      return scalings * vnl_qr<T>(tmp).determinant();
    }
    else
      return vnl_qr<T>(M).determinant();
  }
}

//--------------------------------------------------------------------------------

#undef VNL_DETERMINANT_INSTANTIATE
#define VNL_DETERMINANT_INSTANTIATE(T) \
template T vnl_determinant(T const *, T const *); \
template T vnl_determinant(T const *, T const *, T const *); \
template T vnl_determinant(T const *, T const *, T const *, T const *); \
template T vnl_determinant(T const * const *, int, bool); \
template T vnl_determinant(vnl_matrix<T > const &, bool)


#endif // vnl_algo_determinant_txx_
