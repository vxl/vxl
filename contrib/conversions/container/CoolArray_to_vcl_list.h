#include <cool/ArrayP.h>
#include <vcl_list.h>

template <class T>
inline
vcl_list<T> CoolArray_to_vcl_list(CoolArray<T> const& cl)
{
  vcl_list<T> l;
  CoolArray<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
vcl_list<T> CoolArray_to_vcl_list(CoolArrayP<T> const& cl)
{
  vcl_list<T> l;
  CoolArrayP<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push_back(*it);
  return l;
}
