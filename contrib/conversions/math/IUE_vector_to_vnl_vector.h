#ifndef IUE_vector_to_vnl_vector_h_
#define IUE_vector_to_vnl_vector_h_

#include <math/vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline vnl_vector<T > IUE_vector_to_vnl_vector(IUE_vector<T > const& v)
{
  T const* data = v.data_block();
  return vnl_vector<T >(data, v.size());
}

#endif // IUE_vector_to_vnl_vector_h_
