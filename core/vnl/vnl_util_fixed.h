#ifndef vnl_util_fixed_h_
#define vnl_util_fixed_h_

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>


//: Returns the nxn outer product of two nd-vectors, or [v1]^T*[v2]. O(n).

template<class T, unsigned n>
vnl_matrix_fixed<T, n, n> 
vnl_outer_product_fixed (vnl_vector_fixed<T, n> const& v1,
                         vnl_vector_fixed<T, n> const& v2) {
  vnl_matrix_fixed<T, n, n> out;
  for (unsigned i = 0; i < n; i++)             // v1.column() * v2.row()
    for (unsigned j = 0; j < n; j++)
      out[i][j] = v1[i] * v2[j];
  return out;
}


#endif // vnl_util_fixed_h_
