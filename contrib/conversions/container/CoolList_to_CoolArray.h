#include <cool/ListP.h>
#include <cool/Array.h>

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolList<T> const& cl)
{
  CoolArray<T> l;
  for (cl.reset(); cl.next(); )
    l.push(cl.value());
  return l;
}

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolListP<T> const& cl)
{
  CoolArray<T> l;
  for (cl.reset(); cl.next(); )
    l.push(cl.value());
  return l;
}

template <class T>
inline
CoolArray<T> CoolArrayP_to_CoolArray(CoolArrayP<T> const& cl)
{
  CoolArray<T> l;
  for (cl.reset(); cl.next(); )
    l.push(cl.value());
  return l;
}
