#include <cool/List.h>
#include <vcl_vector.h>

template <class T>
inline
CoolList<T> vcl_vector_to_CoolList(vcl_vector<T> const& cl)
{
  CoolList<T> l;
  vcl_vector<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push_end(*it);
  return l;
}
