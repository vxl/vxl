#include <cool/ListP.h>
#include <vcl_vector.h>

template <class T>
inline
vcl_vector<T> CoolList_to_vcl_vector(CoolList<T> const& cl)
{
  vcl_vector<T> l;
  CoolList<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push_back(*it);
  return l;
}

template <class T>
inline
vcl_vector<T> CoolList_to_vcl_vector(CoolListP<T> const& cl)
{
  vcl_vector<T> l;
  CoolListP<T>::const_iterator it = cl.begin();
  for (; it != cl.end(); ++it)
    l.push_back(*it);
  return l;
}
