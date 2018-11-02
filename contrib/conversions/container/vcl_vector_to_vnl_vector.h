#ifndef vcl_vector_to_vnl_vector_h_
#define vcl_vector_to_vnl_vector_h_

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>

template <class T>
inline
vnl_vector<T> vcl_vector_to_vnl_vector(std::vector<T> const& cl)
{
  vnl_vector<T> l(cl.size());
  typename std::vector<T>::const_iterator it = cl.begin();
  for (int i=0; !(it == cl.end()); ++it, ++i)
    l[i] = *it;
  return l;
}

#endif // vcl_vector_to_vnl_vector_h_
