#ifndef CoolList_to_vcl_list_h_
#define CoolList_to_vcl_list_h_

#include <cool/ListP.h>
#include <vcl_list.h>

template <class T>
inline
vcl_list<T> CoolList_to_vcl_list(CoolList<T> const& cl)
{
  vcl_list<T> l;
  typename CoolList<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
vcl_list<T> CoolList_to_vcl_list(CoolListP<T> const& cl)
{
  vcl_list<T> l;
  typename CoolListP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolList_to_vcl_list_h_
