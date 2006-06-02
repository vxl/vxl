// This is mul/mbl/mbl_data_wrapper.txx
#ifndef mbl_data_wrapper_txx_
#define mbl_data_wrapper_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file

#include "mbl_data_wrapper.h"
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

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
    vcl_cerr<<"TC_VectorDataBase::set_index(n) ";
    vcl_cerr<<"n = "<<n<<" not in range 0 <= n < "<<size()<<vcl_endl;
    vcl_abort();
  }

  if (index()==n) return;
  if (index()>n) reset();
  while (index()!=n) next();
}

#define MBL_DATA_WRAPPER_INSTANTIATE(T) \
template class mbl_data_wrapper< T >

#endif // mbl_data_wrapper_txx_
