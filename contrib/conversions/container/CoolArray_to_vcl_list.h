#include <cool/ArrayP.h>
#include <vcl_list.h>

template <class T>
inline
vcl_list<T> CoolArray_to_vcl_list(CoolArray<T> const& cl)
{
  vcl_list<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}

template <class T>
inline
vcl_list<T> CoolArray_to_vcl_list(CoolArrayP<T> const& cl)
{
  vcl_list<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}
