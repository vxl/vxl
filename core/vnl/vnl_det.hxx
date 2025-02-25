// This is core/vnl/vnl_det.hxx
#ifndef vnl_det_hxx_
#define vnl_det_hxx_

#include "vnl_det.h"

template <class T>
T
vnl_det(const T * row0, const T * row1)
{
  return row0[0] * row1[1] - row0[1] * row1[0];
}

template <class T>
T
vnl_det(const T * row0, const T * row1, const T * row2)
{
  return // the extra '+' makes it work nicely with emacs indentation.
    +row0[0] * row1[1] * row2[2] - row0[0] * row2[1] * row1[2] - row1[0] * row0[1] * row2[2] +
    row1[0] * row2[1] * row0[2] + row2[0] * row0[1] * row1[2] - row2[0] * row1[1] * row0[2];
}

template <class T>
T
vnl_det(const T * row0, const T * row1, const T * row2, const T * row3)
{
  return +row0[0] * row1[1] * row2[2] * row3[3] - row0[0] * row1[1] * row3[2] * row2[3] -
         row0[0] * row2[1] * row1[2] * row3[3] + row0[0] * row2[1] * row3[2] * row1[3] +
         row0[0] * row3[1] * row1[2] * row2[3] - row0[0] * row3[1] * row2[2] * row1[3] -
         row1[0] * row0[1] * row2[2] * row3[3] + row1[0] * row0[1] * row3[2] * row2[3] +
         row1[0] * row2[1] * row0[2] * row3[3] - row1[0] * row2[1] * row3[2] * row0[3] -
         row1[0] * row3[1] * row0[2] * row2[3] + row1[0] * row3[1] * row2[2] * row0[3] +
         row2[0] * row0[1] * row1[2] * row3[3] - row2[0] * row0[1] * row3[2] * row1[3] -
         row2[0] * row1[1] * row0[2] * row3[3] + row2[0] * row1[1] * row3[2] * row0[3] +
         row2[0] * row3[1] * row0[2] * row1[3] - row2[0] * row3[1] * row1[2] * row0[3] -
         row3[0] * row0[1] * row1[2] * row2[3] + row3[0] * row0[1] * row2[2] * row1[3] +
         row3[0] * row1[1] * row0[2] * row2[3] - row3[0] * row1[1] * row2[2] * row0[3] -
         row3[0] * row2[1] * row0[2] * row1[3] + row3[0] * row2[1] * row1[2] * row0[3];
}

//--------------------------------------------------------------------------------

#define VNL_DET_INSTANTIATE(T)                                    \
  template VNL_EXPORT T vnl_det(T const *, T const *);            \
  template VNL_EXPORT T vnl_det(T const *, T const *, T const *); \
  template VNL_EXPORT T vnl_det(T const *, T const *, T const *, T const *)

#endif // vnl_det_hxx_
