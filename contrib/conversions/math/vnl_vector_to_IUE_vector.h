#ifndef vnl_vector_to_IUE_vector_h_
#define vnl_vector_to_IUE_vector_h_

#include <math/vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline IUE_vector<T > vnl_vector_to_IUE_vector(vnl_vector<T > const& v)
{
  T const* data = v.data_block();
  return IUE_vector<T >(data, v.size());
}

#endif // vnl_vector_to_IUE_vector_h_
