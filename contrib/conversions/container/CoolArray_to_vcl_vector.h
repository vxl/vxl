#ifndef CoolArray_to_vcl_vector_h_
#define CoolArray_to_vcl_vector_h_

#include <cool/ArrayP.h>
#include <vcl_vector.h>

template <class T>
inline
vcl_vector<T> CoolArray_to_vcl_vector(CoolArray<T> const& cl)
{
  vcl_vector<T> l;
  typename CoolArray<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
vcl_vector<T> CoolArray_to_vcl_vector(CoolArrayP<T> const& cl)
{
  vcl_vector<T> l;
  typename CoolArrayP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolArray_to_vcl_vector_h_
