// This is vxl/vnl/vnl_matrix_fixed.txx
#ifndef vnl_matrix_fixed_txx_
#define vnl_matrix_fixed_txx_

#include "vnl_matrix_fixed.h"

#define VNL_MATRIX_FIXED_INSTANTIATE(T, M, N) \
template class vnl_matrix_fixed<T ,M ,N >

# undef VNL_MATRIX_FIXED_PAIR_INSTANTIATE
#if !defined(VCL_SUNPRO_CC) && !defined(VCL_WIN32)
# define VNL_MATRIX_FIXED_PAIR_INSTANTIATE(T, M, N, O) \
  template vnl_matrix_fixed<T, M, O> operator*(const vnl_matrix_fixed<T, M, N>& a, const vnl_matrix_fixed<T, N, O>& b)
#else
# define VNL_MATRIX_FIXED_PAIR_INSTANTIATE(T, M, N, O) /* */
#endif

#endif // vnl_matrix_fixed_txx_
