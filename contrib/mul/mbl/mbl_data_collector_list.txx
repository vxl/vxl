#ifndef mbl_data_collector_list_txx_
#define mbl_data_collector_list_txx_

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <mbl/mbl_data_collector_list.h>
#include <vsl/vsl_indent.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

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

template <class T>
vcl_string mbl_data_collector_list<T>::is_a() const
{
  return vcl_string("mbl_data_collector_list<T>");
}

//=======================================================================

template <class T>
short mbl_data_collector_list<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
mbl_data_collector_base* mbl_data_collector_list<T>::clone() const
{
  return new mbl_data_collector_list<T>(*this);
}

template <class T>
void mbl_data_collector_list<T>::print_summary(vcl_ostream& os) const
{
  os<<"Number stored: "<<data_.size()<<vcl_endl;
}

template <class T>
void mbl_data_collector_list<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs, version_no());
  vsl_b_write(bfs, data_);
}

template <class T>
void mbl_data_collector_list<T>::b_read(vsl_b_istream& bfs)
{
  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, data_);
    break;
  default:
    vcl_cerr << "mbl_data_collector_list<T>::b_read() ";
    vcl_cerr << "Unexpected version number " << version << vcl_endl;
    vcl_abort();
  }
}


#define MBL_DATA_COLLECTOR_LIST_INSTANTIATE(T) \
template class mbl_data_collector_list< T >
#endif // mbl_data_collector_list_txx_
