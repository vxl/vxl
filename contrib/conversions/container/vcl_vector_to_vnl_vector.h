#ifndef vcl_vector_to_vnl_vector_h_
#define vcl_vector_to_vnl_vector_h_

#include <vcl_vector.h>
#include <vnl/vnl_vector.h>

template <class T>
inline
vnl_vector<T> vcl_vector_to_vnl_vector(vcl_vector<T> const& cl)
{
  vnl_vector<T> l(cl.size());
  typename vcl_vector<T>::const_iterator it = cl.begin();
  for (int i=0; !(it == cl.end()); ++it, ++i)
    l[i] = *it;
  return l;
}

#endif // vcl_vector_to_vnl_vector_h_
