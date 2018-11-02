#ifndef vcl_vector_to_vcl_list_h_
#define vcl_vector_to_vcl_list_h_

#include <iostream>
#include <list>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
std::list<T> vcl_vector_to_vcl_list(std::vector<T> const& cl)
{
  std::list<T> l;
  typename std::vector<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // vcl_vector_to_vcl_list_h_
