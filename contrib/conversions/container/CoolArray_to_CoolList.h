#include <cool/ArrayP.h>
#include <cool/ListP.h>

template <class T>
inline
CoolList<T> CoolArray_to_CoolList(CoolArray<T> const& cl)
{
  CoolList<T> l;
  for (cl.reset(); cl.next(); )
    l.push_end(cl.value());
  return l;
}

template <class T>
inline
CoolList<T> CoolArray_to_CoolList(CoolArrayP<T> const& cl)
{
  CoolList<T> l;
  for (cl.reset(); cl.next(); )
    l.push_end(cl.value());
  return l;
}

template <class T>
inline
CoolList<T> CoolListP_to_CoolList(CoolListP<T> const& cl)
{
  CoolList<T> l;
  for (cl.reset(); cl.next(); )
    l.push_end(cl.value());
  return l;
}
