#ifndef vnl_matrix_to_IUE_matrix_h_
#define vnl_matrix_to_IUE_matrix_h_

#include <math/matrix.h>
#include <vnl/vnl_matrix.h>

template <class T>
inline IUE_matrix<T > vnl_matrix_to_IUE_matrix(vnl_matrix<T > const& m)
{
  T const* data = m.data_block();
  return IUE_matrix<T >(data, m.rows(), m.columns());
}

#endif // vnl_matrix_to_IUE_matrix_h_
