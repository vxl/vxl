// This is core/vbl/vbl_array_1d.hxx
#ifndef vbl_array_1d_hxx_
#define vbl_array_1d_hxx_

#include <iostream>
#include "vbl_array_1d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class T>
std::ostream& operator<< (std::ostream &os, vbl_array_1d<T> const& array)
{
  os << "vbl_array_1d [";
  for (typename vbl_array_1d<T>::const_iterator i=array.begin();
       i < array.end(); ++i)
    os << ' ' << (*i);
  os << " ]" << std::endl;

  return os;
}

#undef VBL_ARRAY_1D_INSTANTIATE
#define VBL_ARRAY_1D_INSTANTIATE(T) \
template class vbl_array_1d<T >; \
template std::ostream& operator<< (std::ostream& , vbl_array_1d<T > const& )

#endif
