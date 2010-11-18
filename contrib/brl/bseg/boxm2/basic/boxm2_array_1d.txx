// This is brl/bseg/boxm2/basic/boxm2_array_1d.txx
#ifndef boxm2_array_1d_txx_
#define boxm2_array_1d_txx_

#include "boxm2_array_1d.h"
#include <vcl_iostream.h>

template<class T>
vcl_ostream& operator<< (vcl_ostream &os, boxm2_array_1d<T> const& array)
{
  for (typename boxm2_array_1d<T>::const_iterator i=array.begin();
       i < array.end(); ++i)
    os << (*i) << ' ';
  os << vcl_endl;

  return os;
}

#undef BOXM2_ARRAY_1D_INSTANTIATE
#define BOXM2_ARRAY_1D_INSTANTIATE(T) \
template struct boxm2_array_1d<T >; \
template vcl_ostream& operator<< (vcl_ostream& , boxm2_array_1d<T > const& )

#endif
