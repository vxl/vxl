// This is core/vbl/vbl_array_1d.txx
#ifndef vbl_array_1d_txx_
#define vbl_array_1d_txx_

#include "vbl_array_1d.h"

#include <vcl_iostream.h>

template<class T>
vcl_ostream& operator<< (vcl_ostream &os, vbl_array_1d<T> const& array)
{
  for (typename vbl_array_1d<T>::const_iterator i=array.begin();
       i < array.end(); ++i)
    os << (*i) << " ";
  os << vcl_endl;

  return os;
}

#undef VBL_ARRAY_1D_INSTANTIATE
#define VBL_ARRAY_1D_INSTANTIATE(T) \
template struct vbl_array_1d<T >; \
template vcl_ostream& operator<< (vcl_ostream& , vbl_array_1d<T > const& )

#endif
