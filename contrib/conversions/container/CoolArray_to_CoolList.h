#ifndef CoolArray_to_CoolList_h_
#define CoolArray_to_CoolList_h_

#include <cool/ArrayP.h>
#include <cool/ListP.h>

template <class T>
inline
CoolList<T> CoolArray_to_CoolList(CoolArray<T> const& cl)
{
  CoolList<T> l;
  typename CoolArray<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

template <class T>
inline
CoolList<T> CoolArray_to_CoolList(CoolArrayP<T> const& cl)
{
  CoolList<T> l;
  typename CoolArrayP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

template <class T>
inline
CoolList<T> CoolListP_to_CoolList(CoolListP<T> const& cl)
{
  CoolList<T> l;
  typename CoolListP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_end(*it);
  return l;
}

#endif // CoolArray_to_CoolList_h_
