// This is mul/mbl/mbl_file_data_wrapper.txx
#ifndef mbl_file_data_wrapper_txx_
#define mbl_file_data_wrapper_txx_
//:
// \file

#include "mbl_file_data_wrapper.h"

#include <vcl_ios.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h>

// constructor
template<class T>
mbl_file_data_wrapper<T>::mbl_file_data_wrapper(vcl_string path)
: bfs_(path)
{
  path_ = path;
  if (!bfs_)
  {
    vcl_cerr<<"ERROR: mbl_file_data_wrapper::constructor\n"
            <<"file stream failed\n";
    vcl_abort();
  }
  calc_data_size();
  reset();
}

//: Count number of items in file
template<class T>
void mbl_file_data_wrapper<T>::calc_data_size()
{
  unsigned long count=0;

  short version;
  if (!bfs_)
  {
     vcl_cerr<<"ERROR: mbl_file_data_wrapper::calc_data_size()\n"
             <<"file stream failed\n";
     vcl_abort();
  }


  //vcl_cout<<"count= "<<count<<vcl_endl;
  vsl_b_read(bfs_,version);
  T d;
  bool is_last;
  vsl_b_read(bfs_,is_last);
  while (!is_last)
  {
    vsl_b_read(bfs_,d);
    count++;
    //vcl_cout<<"count= "<<count<<vcl_endl;
    vsl_b_read(bfs_,is_last);
#if 0
    vcl_cout << " ***Results1 " << bfs_.is().eof() << ' ' << bfs_.is().bad()
             << ' ' << bfs_.is().fail() << ' ' << bfs_.is().good() << vcl_endl
             << " ***Results2 " << bfs_.is().eof() << ' ' << bfs_.is().bad()
             << ' ' << bfs_.is().fail() << ' ' << bfs_.is().good() << vcl_endl;
#endif
  }

  size_=count;
}


//: Default destructor
template<class T>
mbl_file_data_wrapper<T>::~mbl_file_data_wrapper()
{
}

//: return number of items in file
template<class T>
unsigned long mbl_file_data_wrapper<T>::size() const
{
  return size_;
}


//: Reset
template<class T>
void mbl_file_data_wrapper<T>::reset()
{
  bfs_.is().seekg(vsl_b_ostream::header_length, vcl_ios_beg);
#if 0
  vcl_cout << " ***Results3 " << bfs_.is().eof() << ' ' << bfs_.is().bad()
           << ' ' << bfs_.is().fail() << ' ' << bfs_.is().good() << vcl_endl
           << " ***Results4 " << bfs_.is().eof() << ' ' << bfs_.is().bad()
           << ' ' << bfs_.is().fail() << ' ' << bfs_.is().good() << vcl_endl;
#endif

  if (!bfs_)
  {
    vcl_cerr<<"ERROR: mbl_file_data_wrapper::reset()\n"
            <<"file stream failed\n";
    vcl_abort();
  }

  short version;
  vsl_b_read(bfs_,version);
  bool is_last;
  vsl_b_read(bfs_,is_last);
  if (is_last)
  {
    vcl_cerr<<"ERROR: mbl_file_data_wrapper::reset()\n"
            <<"appears to be no data in file\n";
    vcl_abort();
  }
  else
   vsl_b_read(bfs_,d_);

  index_=0;
}

//: Return current object
template<class T>
const T& mbl_file_data_wrapper<T>::current()
{
  // return current value
  return d_;
}

//: Read in next object
template<class T>
bool mbl_file_data_wrapper<T>::next()
{
  if (!bfs_)
  {
    vcl_cerr<<"ERROR: mbl_file_data_wrapper::next()\n"
            <<"file stream failed\n";
    vcl_abort();
  }

  bool is_last;
  vsl_b_read(bfs_,is_last);
  if (is_last)
  {
    reset();
    //vcl_cout<<"WARNING: Reached end of file, so wrapper has been reset!\n";
    return false;
  }
  else
  {
    vsl_b_read(bfs_,d_);
    index_++;
    return true;
  }
}

//: Return current index
//  First example has index 0
template<class T>
unsigned long mbl_file_data_wrapper<T>::index() const
{
  return index_;
}


#if 0

// an acceptable implementation is defined in mbl_data_wrapper base class!
// maybe just use that??

//: Move to element n
//  First example has index 0
template<class T>
void mbl_file_data_wrapper<T>::set_index(unsigned long n)
{
  // unsupported
  vcl_cout<<"mbl_file_data_wrapper<T>::set_index unsupported\n";
}

#endif


//: Create copy on heap and return base pointer
template<class T>
mbl_data_wrapper< T >* mbl_file_data_wrapper<T>::clone() const
{
  return new mbl_file_data_wrapper<T>(*this);
}

template <class T>
bool mbl_file_data_wrapper<T>::is_class(vcl_string const& s) const
{
  return s==is_a(); // no ref to parent's is_class() since that is pure virtual
}


#define MBL_FILE_DATA_WRAPPER_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mbl_file_data_wrapper<T >::is_a() const \
{ return vcl_string("mbl_file_data_wrapper<" #T ">"); } \
template class mbl_file_data_wrapper<T >

#endif // mbl_file_data_wrapper_txx_
