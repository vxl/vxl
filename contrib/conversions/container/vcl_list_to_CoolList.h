#ifndef std::list_to_CoolList_h_
#define std::list_to_CoolList_h_

#include <iostream>
#include <list>
#include <cool/List.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
CoolList<T> std::list_to_CoolList(std::list<T> const& cl)
{
  CoolList<T> l;
  typename std::list<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

#endif // std::list_to_CoolList_h_
