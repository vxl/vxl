#include <cool/Matrix.h>
#include <vnl/vnl_matrix.h>

template <class T>
inline CoolMatrix<T > vnl_matrix_to_CoolMatrix(vnl_matrix<T > const& m)
{
  T const* data = m.data_block();
  return CoolMatrix<T >(data, m.rows(), m.columns());
}
