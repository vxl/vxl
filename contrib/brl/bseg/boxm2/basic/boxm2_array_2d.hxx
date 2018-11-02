// This is brl/bseg/boxm2/basic/boxm2_array_2d.hxx
#ifndef boxm2_array_2d_hxx_
#define boxm2_array_2d_hxx_

#include <iostream>
#include "boxm2_array_2d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class T>
std::ostream& operator<<(std::ostream &os, boxm2_array_2d<T> const &array)
{
  typedef typename boxm2_array_2d<T>::size_type size_type;
  for ( size_type i=0; i< array.rows(); i++)
  {
    for ( size_type j=0; j< array.columns(); j++)
      os << array(i,j) << ' ';

    os << std::endl;
  }

  return os;
}

#undef BOXM2_ARRAY_2D_INSTANTIATE
#define BOXM2_ARRAY_2D_INSTANTIATE(type) \
template class boxm2_array_2d<type >;\
template std::ostream& operator<< (std::ostream& , boxm2_array_2d<type > const& )

#endif // boxm2_array_2d_hxx_
