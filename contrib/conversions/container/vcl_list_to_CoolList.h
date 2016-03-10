#ifndef std::list_to_CoolList_h_
#define std::list_to_CoolList_h_

#include <cool/List.h>
#include <vcl_compiler.h>
#include <iostream>
#include <list>

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
