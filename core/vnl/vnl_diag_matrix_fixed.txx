// This is core/vnl/vnl_diag_matrix_fixed.txx
// -*- c++ -*-
#ifndef vnl_diag_matrix_fixed_txx_
#define vnl_diag_matrix_fixed_txx_
//:
// \file

#include "vnl_diag_matrix_fixed.h"

#include <vcl_iostream.h>


//: Return inv(D) * b.
template <class T, unsigned int N>
vnl_vector_fixed<T,N> vnl_diag_matrix_fixed<T,N>::solve(vnl_vector_fixed<T,N> const& b) const
{
  vnl_vector_fixed<T,N> ret;
  for (unsigned i = 0; i < N; ++i)
    ret[i] = b[i] / diagonal_[i];
  return ret;
}

//: Return inv(D) * b.
template <class T, unsigned int N>
void vnl_diag_matrix_fixed<T,N>::solve(vnl_vector_fixed<T,N> const& b, vnl_vector_fixed<T,N>* out) const
{
  for (unsigned i = 0; i < N; ++i)
    (*out)[i] = b[i] / diagonal_[i];
}

//: Print in MATLAB diag([1 2 3]) form.
template <class T, unsigned int N>
vcl_ostream& operator<< (vcl_ostream& s, const vnl_diag_matrix_fixed<T,N>& D)
{
  s << "diag([ ";
  for (unsigned i=0; i<N; ++i)
    s << D(i,i) << ' ';
  return s << "])";
}

#undef VNL_DIAG_MATRIX_FIXED_INSTANTIATE
#define VNL_DIAG_MATRIX_FIXED_INSTANTIATE(T , N ) \
template class vnl_diag_matrix_fixed<T , N >; \
template vcl_ostream& operator<< (vcl_ostream& s, vnl_diag_matrix_fixed<T , N > const &)

//template bool epsilon_equals (vnl_diag_matrix_fixed<T > const & , vnl_diag_matrix_fixed<T > const & , double)

#endif // vnl_diag_matrix_fixed_txx_
