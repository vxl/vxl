#ifndef CoolList_to_vcl_list_h_
#define CoolList_to_vcl_list_h_

#include <iostream>
#include <list>
#include <cool/ListP.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
std::list<T> CoolList_to_vcl_list(CoolList<T> const& cl)
{
  std::list<T> l;
  typename CoolList<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
std::list<T> CoolList_to_vcl_list(CoolListP<T> const& cl)
{
  std::list<T> l;
  typename CoolListP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolList_to_vcl_list_h_
