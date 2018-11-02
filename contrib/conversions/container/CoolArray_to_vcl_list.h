#ifndef CoolArray_to_vcl_list_h_
#define CoolArray_to_vcl_list_h_

#include <iostream>
#include <list>
#include <cool/ArrayP.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
std::list<T> CoolArray_to_vcl_list(CoolArray<T> const& cl)
{
  std::list<T> l;
  typename CoolArray<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
std::list<T> CoolArray_to_vcl_list(CoolArrayP<T> const& cl)
{
  std::list<T> l;
  typename CoolArrayP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolArray_to_vcl_list_h_
