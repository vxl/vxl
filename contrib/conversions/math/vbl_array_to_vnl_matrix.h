#ifndef vbl_array_to_vnl_matrix_h_
#define vbl_array_to_vnl_matrix_h_

#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_2d.h>

template <class T>
inline vnl_matrix<T> vbl_array_to_vnl_matrix(vbl_array_2d<T> a)
{
  vnl_matrix<T> m(a.rows(), a.columns());
  T* i = a.begin();
  T* j = m.begin();
  for ( ; !(i==a.end()); ++i,++j)
    *j = *i;
  return m;
}

#endif // vbl_array_to_vnl_matrix_h_
