#ifndef vcl_vector_to_vcl_list_h_
#define vcl_vector_to_vcl_list_h_

#include <vcl_list.h>
#include <vcl_vector.h>

template <class T>
inline
vcl_list<T> vcl_vector_to_vcl_list(vcl_vector<T> const& cl)
{
  vcl_list<T> l;
  typename vcl_vector<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // vcl_vector_to_vcl_list_h_
