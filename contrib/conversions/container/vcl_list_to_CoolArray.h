#ifndef vcl_list_to_CoolArray_h_
#define vcl_list_to_CoolArray_h_

#include <cool/ArrayP.h>
#include <vcl_list.h>

template <class T>
inline
CoolArray<T> vcl_list_to_CoolArray(vcl_list<T> const& cl)
{
  CoolArray<T> l;
  typename vcl_list<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push(*it);
  return l;
}

#endif // vcl_list_to_CoolArray_h_
