#include <cool/ListP.h>
#include <cool/Array.h>

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolList<T> const& cl)
{
  CoolArray<T> l;
  CoolList<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push(*it);
  return l;
}

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolListP<T> const& cl)
{
  CoolArray<T> l;
  CoolListP<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push(*it);
  return l;
}

template <class T>
inline
CoolArray<T> CoolArrayP_to_CoolArray(CoolArrayP<T> const& cl)
{
  CoolArray<T> l;
  CoolArrayP<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push(*it);
  return l;
}
