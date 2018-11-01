// This is mul/mbl/mbl_data_wrapper.hxx
#ifndef mbl_data_wrapper_hxx_
#define mbl_data_wrapper_hxx_
//:
//  \file

#include <iostream>
#include <cstdlib>
#include "mbl_data_wrapper.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Default constructor
template<class T>
mbl_data_wrapper<T>::mbl_data_wrapper() = default;

//: Default destructor
template<class T>
mbl_data_wrapper<T>::~mbl_data_wrapper() = default;

//: Move to element n
//  First example has index 0
template<class T>
void mbl_data_wrapper<T>::set_index(unsigned long n)
{
  if ((n>=size()))
  {
    std::cerr<<"TC_VectorDataBase::set_index(n) ";
    std::cerr<<"n = "<<n<<" not in range 0 <= n < "<<size()<<std::endl;
    std::abort();
  }

  if (index()==n) return;
  if (index()>n) reset();
  while (index()!=n) next();
}

#define MBL_DATA_WRAPPER_INSTANTIATE(T) \
template class mbl_data_wrapper< T >

#endif // mbl_data_wrapper_hxx_
