#ifndef vnl_vector_to_CoolVector_h_
#define vnl_vector_to_CoolVector_h_

#include <cool/Vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline CoolVector<T > vnl_vector_to_CoolVector(vnl_vector<T > const& v)
{
  T const* data = v.data_block();
  return CoolVector<T >(data, v.size());
}

#endif // vnl_vector_to_CoolVector_h_
