#ifndef CoolArray_to_vcl_vector_h_
#define CoolArray_to_vcl_vector_h_

#include <iostream>
#include <vector>
#include <cool/ArrayP.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
std::vector<T> CoolArray_to_vcl_vector(CoolArray<T> const& cl)
{
  std::vector<T> l;
  typename CoolArray<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
std::vector<T> CoolArray_to_vcl_vector(CoolArrayP<T> const& cl)
{
  std::vector<T> l;
  typename CoolArrayP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolArray_to_vcl_vector_h_
