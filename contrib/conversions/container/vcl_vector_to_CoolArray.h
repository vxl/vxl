#include <cool/Array.h>
#include <vcl_vector.h>

template <class T>
inline
CoolArray<T> vcl_vector_to_CoolArray(vcl_vector<T> const& cl)
{
  CoolArray<T> l;
  vcl_vector<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push(*it);
  return l;
}
