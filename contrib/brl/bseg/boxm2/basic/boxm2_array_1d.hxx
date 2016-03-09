// This is brl/bseg/boxm2/basic/boxm2_array_1d.hxx
#ifndef boxm2_array_1d_hxx_
#define boxm2_array_1d_hxx_

#include "boxm2_array_1d.h"
#include <vcl_compiler.h>
#include <iostream>
#include <iostream>

template<class T>
std::ostream& operator<< (std::ostream &os, boxm2_array_1d<T> const& array)
{
  for (typename boxm2_array_1d<T>::const_iterator i=array.begin();
       i < array.end(); ++i)
    os << (*i) << ' ';
  os << std::endl;

  return os;
}

#undef BOXM2_ARRAY_1D_INSTANTIATE
#define BOXM2_ARRAY_1D_INSTANTIATE(T) \
template struct boxm2_array_1d<T >; \
template std::ostream& operator<< (std::ostream& , boxm2_array_1d<T > const& )

#endif
