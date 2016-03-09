#ifndef std::vector_to_CoolList_h_
#define std::vector_to_CoolList_h_

#include <cool/List.h>
#include <vcl_compiler.h>
#include <iostream>
#include <vector>

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
