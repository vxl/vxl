#include <math/vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline vnl_vector<T > IUE_vector_to_vnl_vector(IUE_vector<T > const& v)
{
  T const* data = v.data_block();
  return vnl_vector<T >(data, v.size());
}
