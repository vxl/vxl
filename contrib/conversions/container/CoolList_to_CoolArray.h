#ifndef CoolList_to_CoolArray_h_
#define CoolList_to_CoolArray_h_

#include <cool/ListP.h>
#include <cool/Array.h>

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolList<T> const& cl)
{
  CoolArray<T> l;
  typename CoolList<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push(*it);
  return l;
}

template <class T>
inline
CoolArray<T> CoolList_to_CoolArray(CoolListP<T> const& cl)
{
  CoolArray<T> l;
  typename CoolListP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push(*it);
  return l;
}

template <class T>
inline
CoolArray<T> CoolArrayP_to_CoolArray(CoolArrayP<T> const& cl)
{
  CoolArray<T> l;
  typename CoolArrayP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push(*it);
  return l;
}

#endif // CoolList_to_CoolArray_h_
