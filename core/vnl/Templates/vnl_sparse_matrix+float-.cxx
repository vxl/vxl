#include <vnl/vnl_sparse_matrix.txx>

template class vnl_sparse_matrix<float>;

static void vnl_sparse_matrix_float_tickler()
{
  vnl_sparse_matrix<float> md(3, 5);
}
