// This is brl/bseg/boxm2/basic/boxm2_array_2d.txx
#ifndef boxm2_array_2d_txx_
#define boxm2_array_2d_txx_

#include "boxm2_array_2d.h"

#include <vcl_iostream.h>

template<class T>
vcl_ostream& operator<<(vcl_ostream &os, boxm2_array_2d<T> const &array)
{
  typedef typename boxm2_array_2d<T>::size_type size_type;
  for ( size_type i=0; i< array.rows(); i++)
  {
    for ( size_type j=0; j< array.columns(); j++)
      os << array(i,j) << ' ';

    os << vcl_endl;
  }

  return os;
}

#undef BOXM2_ARRAY_2D_INSTANTIATE
#define BOXM2_ARRAY_2D_INSTANTIATE(type) \
template class boxm2_array_2d<type >;\
template vcl_ostream& operator<< (vcl_ostream& , boxm2_array_2d<type > const& )

#endif // boxm2_array_2d_txx_
