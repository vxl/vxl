#include <vnl/vnl_vector.h>
#include <vbl/vbl_array_1d.h>



template <class T>
inline vnl_vector<T> vbl_array_to_vnl_vector(vbl_array_1d<T> a)
{
  return vnl_vector<T>(a.begin(), a.end()-a.begin());
}
