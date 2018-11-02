// This is core/vbl/vbl_array_2d.hxx
#ifndef vbl_array_2d_hxx_
#define vbl_array_2d_hxx_

#include <iostream>
#include "vbl_array_2d.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template<class T>
std::ostream& operator<<(std::ostream &os, vbl_array_2d<T> const &array)
{
  typedef typename vbl_array_2d<T>::size_type size_type;
  os << "vbl_array_2d [";
  for ( size_type i=0; i< array.rows(); i++)
  {
    os << std::endl << "   ";
    for ( size_type j=0; j< array.columns(); j++)
      os << ' ' << array(i,j);
  }
  os << "\n             ]" << std::endl;

  return os;
}

#undef VBL_ARRAY_2D_INSTANTIATE
#define VBL_ARRAY_2D_INSTANTIATE(type) \
template class vbl_array_2d<type >;\
template std::ostream& operator<< (std::ostream& , vbl_array_2d<type > const& )

#endif // vbl_array_2d_hxx_
