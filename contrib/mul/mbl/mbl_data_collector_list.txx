#ifndef mbl_data_collector_list_txx_
#define mbl_data_collector_list_txx_

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <mbl/mbl_data_collector_list.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>

//=======================================================================
// Dflt ctor
//=======================================================================

template<class T>
mbl_data_collector_list<T>::mbl_data_collector_list()
{
}

//=======================================================================
// Destructor
//=======================================================================

template<class T>
mbl_data_collector_list<T>::~mbl_data_collector_list()
{
}

//: Clear any stored data
template<class T>
void mbl_data_collector_list<T>::clean()
{
  data_.resize(0);
}

//: Hint about how many examples to expect
template<class T>
void mbl_data_collector_list<T>::setNSamples(int)
{
  // ignored.
}

  //: Record given object
template<class T>
void mbl_data_collector_list<T>::record(const T& d)
{
  data_.push_back(d);
}

//: Return object describing stored data
template<class T>
mbl_data_wrapper<T >& mbl_data_collector_list<T>::data_wrapper()
{
  wrapper_.set(data_.begin(),data_.size());
  return wrapper_;
}

#define MBL_DATA_COLLECTOR_LIST_INSTANTIATE(T) \
template class mbl_data_collector_list< T >
#endif // mbl_data_collector_list_txx_
