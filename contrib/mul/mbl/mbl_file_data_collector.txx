// This is mul/mbl/mbl_file_data_collector.txx
#ifndef mbl_file_data_collector_txx_
#define mbl_file_data_collector_txx_
//:
// \file

#include "mbl_file_data_collector.h"

#include <vcl_cstdlib.h>
#include <vcl_cassert.h>
#include <vsl/vsl_binary_loader.h>
#include <vsl/vsl_vector_io.h>

//=======================================================================
// Constructor
//=======================================================================

template<class T>
mbl_file_data_collector<T>::mbl_file_data_collector(vcl_string& path)
: bfs_(path), wrapper_(0)
{
  path_ = path;
  short vn= 1;
  if (!bfs_)
  {
    vcl_cerr<<"ERROR: mbl_file_data_collector::constructor\n"
            <<"file stream failed\n";
    vcl_abort();
  }

  vsl_b_write(bfs_, vn);
}

#if 0 // commented out

//Can't make copy constructor because have to initialize binary file stream
//some how. DON'T know how to do that!

//: Copy constructor
template<class T>
mbl_file_data_collector<T>::mbl_file_data_collector(const mbl_data_collector_base& c)
{
  assert( c.is_class("mbl_file_data_collector<T>") );
  *this= c;
}

//: Copy operator
template<class T>
mbl_file_data_collector<T>& mbl_file_data_collector<T>::
      operator=( const mbl_data_collector_base& c)
{
  assert( c.is_a()="mbl_file_data_collector<T>" );
  mbl_file_data_collector<T>& cref=dynamic_cast< mbl_file_data_collector<T>& > c;

  // I think I just need to clone the wrapper
  delete_stuff();

  bfs_= cref.bfs_;
  path_ = cref.path_;
  wrapper_ = cref.wrapper_->clone();

  return *this;
}

#endif // 0

//=======================================================================
// Destructor
//=======================================================================

template<class T>
mbl_file_data_collector<T>::~mbl_file_data_collector()
{
  delete_stuff();

  //bfs_.close();
  //delete wrapper_;
}

//: Delete stuff
template<class T>
void mbl_file_data_collector<T>::delete_stuff()
{
  bfs_.close();
  delete wrapper_;
  wrapper_ = 0;
}

//: Clear any stored data
template<class T>
void mbl_file_data_collector<T>::clear()
{
  // can't clear (need to wipe data file to do a proper clear ???)
  vcl_cout<<"mbl_file_data_collector<T>::clear - no action taken\n"
          <<"can't delete data file\n";
}

//: Hint about how many examples to expect
template<class T>
void mbl_file_data_collector<T>::set_n_samples(int /*n*/)
{
  vcl_cerr << "mbl_file_data_collector::set_n_samples() is not useful\n";
}

//: Record given object
template<class T>
void mbl_file_data_collector<T>::record(const T& d)
{
  if (!bfs_)
  {
    vcl_cerr<<"ERROR: mbl_file_data_collector::record()\n"
            <<"file stream failed\n";
    vcl_abort();
  }
  else
  {
    vsl_b_write(bfs_, false);
    vsl_b_write(bfs_, d);
  }
}

//: Return object describing the stored data
template<class T>
mbl_data_wrapper<T >& mbl_file_data_collector<T>::data_wrapper()
{
  // have to say (like Jim Morrison) "this is the end"
  vsl_b_write(bfs_, true);

  // flush the file (to make sure it exists on disk - i.e. override buffering)
  bfs_.os().flush();

  if (!wrapper_)
    wrapper_ = new mbl_file_data_wrapper<T>(path_) ;

  wrapper_->reset();  // make sure points to start!

  return *wrapper_;
}

template <class T>
bool mbl_file_data_collector<T>::is_class(vcl_string const& s) const
{
  return s==mbl_file_data_collector<T>::is_a() || mbl_data_collector<T>::is_class(s);
}

//=======================================================================

template <class T>
short mbl_file_data_collector<T>::version_no() const
{
  return 1;
}

//=======================================================================

template <class T>
mbl_data_collector_base* mbl_file_data_collector<T>::clone() const
{
  vcl_cout<<"ERROR: mbl_file_data_collector<T>::clone()\n"
          <<"Can't clone this class\n";
  vcl_abort();

  // can't find a way of writing copy constructor! so don't allow clone
  //return new mbl_file_data_collector<T>(*this);
  return 0;
}

template <class T>
void mbl_file_data_collector<T>::print_summary(vcl_ostream& os) const
{
  os<<"Data saved to: "<<path_<<vcl_endl;
}

template <class T>
void mbl_file_data_collector<T>::b_write(vsl_b_ostream& /*bfs*/) const
{
  vcl_cout<<"mbl_file_data_collector<T>::b_write - Can't save collector!\n";
}

template <class T>
void mbl_file_data_collector<T>::b_read(vsl_b_istream& /*bfs*/)
{
  vcl_cout<<"mbl_file_data_collector<T>::b_read - Can't load collector!\n";
}


#define MBL_FILE_DATA_COLLECTOR_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mbl_file_data_collector<T >::is_a() const \
{ return vcl_string("mbl_file_data_collector<" #T ">"); } \
template class mbl_file_data_collector<T >

#endif // mbl_file_data_collector_txx_
