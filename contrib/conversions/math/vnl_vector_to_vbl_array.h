#ifndef vnl_vector_to_vbl_array_h_
#define vnl_vector_to_vbl_array_h_

#include <vnl/vnl_vector.h>
#include <vbl/vbl_array_1d.h>

template <class T>
inline vbl_array_1d<T> vnl_vector_to_vbl_array(vnl_vector<T> v)
{
  return vbl_array_1d<T>(v.begin(), v.end());
}

#endif // vnl_vector_to_vbl_array_h_
