/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vnl_determinant.h"

#include <vcl/vcl_cassert.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_copy.h>
#include <vnl/algo/vnl_qr.h>

template <class T>
T vnl_determinant(T const * const * rows, unsigned n) {
  switch (n) {
  case 1: return rows[0][0];
  case 2: return rows[0][0]*rows[1][1] - rows[0][1]*rows[1][0];
  case 3: return
	    + rows[0][0]*rows[1][1]*rows[2][2]
	    - rows[0][0]*rows[1][2]*rows[2][1]
	    + rows[1][0]*rows[0][2]*rows[2][1]
	    - rows[1][0]*rows[0][1]*rows[2][2]
	    + rows[2][0]*rows[0][1]*rows[1][2]
	    - rows[2][0]*rows[0][2]*rows[1][1];
  default: 
    { // for largish matrices it's better to use a matrix decomposition.
      vnl_matrix<T> tmp(n,n); // copy, not ref, as we can't assume the rows are contiguous
      for (unsigned i=0; i<n; ++i)
	tmp.set_row(i, rows[i]);
      return vnl_determinant(tmp);
    }
  }
}

static float qr_det(vnl_matrix<float> const &M) {
  vnl_matrix<double> dM(M.rows(), M.cols());
  vnl_copy(M, dM);
  return float( vnl_qr(dM).determinant() );
}

static double qr_det(vnl_matrix<double> const &M) {
  return vnl_qr(M).determinant();
}

template <class T>
T vnl_determinant(vnl_matrix<T> const &M) {
  unsigned n = M.rows();
  vcl_assert(M.cols() == n);
  if (n<4)
    return vnl_determinant(M.data_array(), n);
  else {
    // will work for real types only. FIXME
    return ::qr_det(M);
  }
}

//--------------------------------------------------------------------------------

#define VNL_DETERMINANT_INSTANTIATE(T) \
template T vnl_determinant(T const * const *, unsigned); \
template T vnl_determinant(vnl_matrix<T> const &);

VNL_DETERMINANT_INSTANTIATE(float);
VNL_DETERMINANT_INSTANTIATE(double);
