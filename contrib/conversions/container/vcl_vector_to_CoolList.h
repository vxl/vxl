#ifndef vcl_vector_to_CoolList_h_
#define vcl_vector_to_CoolList_h_

#include <cool/List.h>
#include <vcl_vector.h>

template <class T>
inline
CoolList<T> vcl_vector_to_CoolList(vcl_vector<T> const& cl)
{
  CoolList<T> l;
  typename vcl_vector<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

#endif // vcl_vector_to_CoolList_h_
