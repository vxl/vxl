#include <cool/ListP.h>
#include <vcl_list.h>

template <class T>
inline
vcl_list<T> CoolList_to_vcl_list(CoolList<T> const& c)
{
  CoolList<T>& cl = (CoolList<T>&)c;
  vcl_list<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}

template <class T>
inline
vcl_list<T> CoolList_to_vcl_list(CoolListP<T> const& c)
{
  CoolListP<T>& cl = (CoolListP<T>&)c;
  vcl_list<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}
