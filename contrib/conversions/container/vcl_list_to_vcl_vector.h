#ifndef vcl_list_to_vcl_vector_h_
#define vcl_list_to_vcl_vector_h_

#include <vcl_vector.h>
#include <vcl_list.h>

template <class T>
inline
vcl_vector<T> vcl_list_to_vcl_vector(vcl_list<T> const& cl)
{
  vcl_vector<T> l;
  typename vcl_list<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // vcl_list_to_vcl_vector_h_
