#ifndef vnl_sym_matrix_txx_
#define vnl_sym_matrix_txx_

#include "vnl_sym_matrix.h"

#include <vcl_iostream.h>


//: print in lower triangular form
template <class T>
vcl_ostream& operator<< (vcl_ostream& s, const vnl_sym_matrix<T>& M)
{
  for (unsigned i=0; i<M.rows(); ++i)
  {
    for (unsigned j=0; j<=i; ++j)
      s << M.fast(i,j) << " ";
    s  << vcl_endl;
  }
  return s;
}


template <class T>
bool operator==(const vnl_sym_matrix<T> &a, const vnl_sym_matrix<T> &b)
{
  if (a.rows() != b.rows()) return false;
  const T* a_data = a.data_block();
  const T* b_data = a.data_block();
  const unsigned mn = a.size();
  for (unsigned i = 0; i < mn; ++i)
    if (a_data[i] != b_data[i]) return false;
  return true;
}


template <class T>
bool operator==(const vnl_sym_matrix<T> &a, const vnl_matrix<T> &b)
{
  if (a.rows() != b.rows() || a.cols() != b.cols()) return false;
  const T* a_data = a.data_block();
  const T* b_data = a.data_block();
  const unsigned n = a.rows();
  for (unsigned i=0; i< n; ++i)
    for (unsigned j=0; j<=i; ++j)
      if (a.fast(i,j) != b(i,j) || a.fast(i,j) != b(j,i)) return false;
  return true;
}

template <class T>
bool operator==(const vnl_matrix<T> &a, const vnl_sym_matrix<T> &b)
{
  return operator==(b,a);
}


#undef VNL_SYM_MATRIX_INSTANTIATE
#define VNL_SYM_MATRIX_INSTANTIATE(T) \
template class vnl_sym_matrix<T >; \
template vcl_ostream& operator<< (vcl_ostream& s, vnl_sym_matrix<T > const &); \
template bool operator==(const vnl_sym_matrix<T > &a, const vnl_sym_matrix<T > &b); \
template bool operator==(const vnl_sym_matrix<T > &a, const vnl_matrix<T > &b); \
template bool operator==(const vnl_matrix<T > &a, const vnl_sym_matrix<T > &b)
#endif
