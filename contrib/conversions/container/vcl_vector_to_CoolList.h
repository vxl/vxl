#ifndef std::vector_to_CoolList_h_
#define std::vector_to_CoolList_h_

#include <iostream>
#include <vector>
#include <cool/List.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
CoolList<T> std::vector_to_CoolList(std::vector<T> const& cl)
{
  CoolList<T> l;
  typename std::vector<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

#endif // std::vector_to_CoolList_h_
