#include <cool/ArrayP.h>
#include <vcl_vector.h>

template <class T>
inline
vcl_vector<T> CoolArray_to_vcl_vector(CoolArray<T> const& c)
{
  CoolArray<T>& cl = (CoolArray<T>&)c;
  vcl_vector<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}

template <class T>
inline
vcl_vector<T> CoolArray_to_vcl_vector(CoolArrayP<T> const& c)
{
  CoolArrayP<T>& cl = (CoolArrayP<T>&)c;
  vcl_vector<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}
