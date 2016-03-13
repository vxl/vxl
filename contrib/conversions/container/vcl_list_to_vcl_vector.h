#ifndef vcl_list_to_vcl_vector_h_
#define vcl_list_to_vcl_vector_h_

#include <vector>
#include <iostream>
#include <list>
#include <vcl_compiler.h>

template <class T>
inline
std::vector<T> vcl_list_to_vcl_vector(std::list<T> const& cl)
{
  std::vector<T> l;
  typename std::list<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // vcl_list_to_vcl_vector_h_
