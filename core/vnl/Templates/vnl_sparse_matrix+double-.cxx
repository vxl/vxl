#include <vnl/vnl_sparse_matrix.txx>

template class vnl_sparse_matrix<double>;

static void vnl_sparse_matrix_double_tickler()
{
  vnl_sparse_matrix<double> md(3, 5);
}
