// This is mul/mbl/mbl_data_collector_list.hxx
#ifndef mbl_data_collector_list_hxx_
#define mbl_data_collector_list_hxx_
//:
//  \file

#include "mbl_data_collector_list.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Dflt ctor
//=======================================================================

template<class T>
mbl_data_collector_list<T>::mbl_data_collector_list() = default;

//=======================================================================
// Destructor
//=======================================================================

template<class T>
mbl_data_collector_list<T>::~mbl_data_collector_list() = default;

//: Clear any stored data
template<class T>
void mbl_data_collector_list<T>::clear()
{
  data_.resize(0);
}

//: Hint about how many examples to expect
template<class T>
void mbl_data_collector_list<T>::set_n_samples(int n)
{
  data_.reserve(n);
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
  if (data_.size() > 0)
  {
    // Check assumption that std::vectors store their data in a contiguous block of memory
    assert(&data_[data_.size()-1] + 1 == &data_[0] + data_.size());

    wrapper_.set(&data_[0],data_.size()); // not data_.begin() since set() expects T*, not vector_iterator
  }
  else
    wrapper_.set(nullptr, 0);
  return wrapper_;
}


template <class T>
bool mbl_data_collector_list<T>::is_class(std::string const& s) const
{
  return s==mbl_data_collector_list<T>::is_a() || mbl_data_collector<T>::is_class(s);
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
void mbl_data_collector_list<T>::print_summary(std::ostream& os) const
{
  os<<"Number stored: "<<data_.size()<<'\n';
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
  if (!bfs) return;

  short version;
  vsl_b_read(bfs,version);
  switch (version)
  {
  case (1):
    vsl_b_read(bfs, data_);
    break;
  default:
    std::cerr << "I/O ERROR: mbl_data_collector_list<T>::b_read(vsl_b_istream&)\n";
    std::cerr << "           Unknown version number "<< version << "\n";
    bfs.is().clear(std::ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


#define MBL_DATA_COLLECTOR_LIST_INSTANTIATE(T) \
template <> std::string mbl_data_collector_list<T >::is_a() const \
{  return std::string("mbl_data_collector_list<" #T ">"); }\
template class mbl_data_collector_list< T >
#endif // mbl_data_collector_list_hxx_
