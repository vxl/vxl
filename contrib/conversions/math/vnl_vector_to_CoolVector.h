#include <cool/Vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline CoolVector<T > vnl_vector_to_CoolVector(vnl_vector<T > const& v)
{
  T const* data = v.data_block();
  return CoolVector<T >(data, v.size());
}
