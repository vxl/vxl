#include <cool/ListP.h>
#include <vcl_vector.h>

template <class T>
inline
vcl_vector<T> CoolList_to_vcl_vector(CoolList<T> const& cl)
{
  vcl_vector<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}

template <class T>
inline
vcl_vector<T> CoolList_to_vcl_vector(CoolListP<T> const& cl)
{
  vcl_vector<T> l;
  for (cl.reset(); cl.next(); )
    l.push_back(cl.value());
  return l;
}
