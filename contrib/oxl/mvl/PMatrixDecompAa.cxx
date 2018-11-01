// This is oxl/mvl/PMatrixDecompAa.cxx

#include "PMatrixDecompAa.h"

void
PMatrixDecompAa::set(const vnl_matrix<double>& P)
{
  for (int r = 0; r < 3; ++r) {
    for (int c = 0; c < 3; ++c)
      A(r,c) = P(r,c);
    a[r] = P(r,3);
  }
}
