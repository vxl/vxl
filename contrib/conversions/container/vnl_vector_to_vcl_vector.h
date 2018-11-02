#ifndef vnl_vector_to_vcl_vector_h_
#define vnl_vector_to_vcl_vector_h_

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>

template <class T>
inline
std::vector<T> vnl_vector_to_vcl_vector(vnl_vector<T> const& cl)
{
  std::vector<T> l; l.reserve(cl.size());
  for (unsigned int i=0; i < cl.size(); ++i)
    l.push_back(cl[i]);
  return l;
}

#endif // vnl_vector_to_vcl_vector_h_
