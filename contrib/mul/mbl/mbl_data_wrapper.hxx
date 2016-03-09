// This is mul/mbl/mbl_data_wrapper.hxx
#ifndef mbl_data_wrapper_hxx_
#define mbl_data_wrapper_hxx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file

#include "mbl_data_wrapper.h"
#include <iostream>
#include <vcl_compiler.h>
#include <iostream>
#include <cstdlib>

//: Default constructor
template<class T>
mbl_data_wrapper<T>::mbl_data_wrapper()
{
}

//: Default destructor
template<class T>
mbl_data_wrapper<T>::~mbl_data_wrapper()
{
}

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
