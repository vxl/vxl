#ifndef vcl_list_to_CoolList_h_
#define vcl_list_to_CoolList_h_

#include <cool/List.h>
#include <vcl_list.h>

template <class T>
inline
CoolList<T> vcl_list_to_CoolList(vcl_list<T> const& cl)
{
  CoolList<T> l;
  typename vcl_list<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

#endif // vcl_list_to_CoolList_h_
