#include <iostream>
#include "vnl/vnl_sparse_matrix.h"
#include "vnl/vnl_random.h"

int
main()
{
  constexpr int N = 100;
  vnl_sparse_matrix<double> a(N, N);
  vnl_random rg;

  for (int i = 0; i < 10; ++i)
  {
    int x = rg.lrand32(N - 1);
    int y = rg.lrand32(N - 1);
    a(x, y) = a(y, x) = 10 * rg.normal(); // symmetric matrix
  }

  a.normalize_rows();
  a = a * a; // i.e., also a * aT
  return 0;
}
