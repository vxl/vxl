#ifndef vnl_matrix_to_vbl_array_h_
#define vnl_matrix_to_vbl_array_h_

#include <vnl/vnl_matrix.h>
#include <vbl/vbl_array_2d.h>

template <class T>
inline vbl_array_2d<T> vnl_matrix_to_vbl_array(vnl_matrix<T> m)
{
  vbl_array_2d<T> a(m.rows(), m.columns());
  T* i = m.begin();
  T* j = a.begin();
  for ( ; !(i==m.end()); ++i,++j)
    *j = *i;
  return a;
}

#endif // vnl_matrix_to_vbl_array_h_
