#include <math/vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline IUE_vector<T > vnl_vector_to_IUE_vector(vnl_vector<T > const& v)
{
  T const* data = v.data_block();
  return IUE_vector<T >(data, v.size());
}
