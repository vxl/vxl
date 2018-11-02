#ifndef CoolList_to_vcl_vector_h_
#define CoolList_to_vcl_vector_h_

#include <iostream>
#include <vector>
#include <cool/ListP.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
inline
std::vector<T> CoolList_to_vcl_vector(CoolList<T> const& cl)
{
  std::vector<T> l;
  typename CoolList<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
std::vector<T> CoolList_to_vcl_vector(CoolListP<T> const& cl)
{
  std::vector<T> l;
  typename CoolListP<T>::const_iterator it = cl.begin();
  for (; !(it == cl.end()); ++it)
    l.push_back(*it);
  return l;
}

#endif // CoolList_to_vcl_vector_h_
