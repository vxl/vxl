#ifndef IUE_matrix_to_vnl_matrix_h_
#define IUE_matrix_to_vnl_matrix_h_

#include <math/matrix.h>
#include <vnl/vnl_matrix.h>

template <class T>
inline vnl_matrix<T > IUE_matrix_to_vnl_matrix(IUE_matrix<T > const& m)
{
  T const* data = m.data_block();
  return vnl_matrix<T >(data, m.rows(), m.columns());
}

#endif // IUE_matrix_to_vnl_matrix_h_
