// This is vxl/vnl/vnl_determinant.txx
#ifndef vnl_determinant_txx_
#define vnl_determinant_txx_

/*
  fsm@robots.ox.ac.uk
*/
#include "vnl_determinant.h"

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

#define VNL_DETERMINANT_INSTANTIATE(T) \
template T vnl_determinant(T const *, T const *); \
template T vnl_determinant(T const *, T const *, T const *); \
template T vnl_determinant(T const *, T const *, T const *, T const *)

#endif // vnl_determinant_txx_
