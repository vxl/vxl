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


#undef VNL_SYM_MATRIX_INSTANTIATE
#define VNL_SYM_MATRIX_INSTANTIATE(T) \
template class vnl_sym_matrix<T >; \
template vcl_ostream& operator<< (vcl_ostream& s, vnl_sym_matrix<T > const &)


#endif
