// This is core/vbl/vbl_array_2d.txx
#ifndef vbl_array_2d_txx_
#define vbl_array_2d_txx_

#include "vbl_array_2d.h"

#include <vcl_iostream.h>

template<class T>
vcl_ostream& operator<<(vcl_ostream &os, vbl_array_2d<T> const &array)
{
  for ( int i=0; i< array.rows(); i++)
  {
    for ( int j=0; j< array.columns(); j++)
      os << array(i,j) << ' ';

    os << vcl_endl;
  }

  return os;
}

#undef VBL_ARRAY_2D_INSTANTIATE
#define VBL_ARRAY_2D_INSTANTIATE(type) \
template class vbl_array_2d<type >;\
template vcl_ostream& operator<< (vcl_ostream& , vbl_array_2d<type > const& )

#endif // vbl_array_2d_txx_
