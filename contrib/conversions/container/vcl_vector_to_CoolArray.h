#ifndef std::vector_to_CoolArray_h_
#define std::vector_to_CoolArray_h_

#include <iostream>
#include <vector>
#include <cool/Array.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
CoolArray<T> std::vector_to_CoolArray(std::vector<T> const& cl)
{
  CoolArray<T> l;
  typename std::vector<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push(*it);
  return l;
}

#endif // std::vector_to_CoolArray_h_
