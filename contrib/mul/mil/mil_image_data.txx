// This is mul/mil/mil_image_data.txx
#ifndef mil_image_data_txx_
#define mil_image_data_txx_
//:
// \file
// \brief Ref. counted block of data on the heap
// \author Tim Cootes

#include "mil_image_data.h"

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h>

//: Dflt ctor
template<class T>
mil_image_data<T>::mil_image_data()
: data_(0), size_(0), ref_count_(0)
{
}

//: Destructor
template<class T>
mil_image_data<T>::~mil_image_data()
{
  delete [] data_;
}

//: Copy ctor
template<class T>
mil_image_data<T>::mil_image_data(const mil_image_data& d)
: data_(0), size_(0), ref_count_(0)
{
  *this=d;
}

//: Copy operator
template<class T>
mil_image_data<T>& mil_image_data<T>::operator=(const mil_image_data& d)
{
  if (this==&d) return *this;

  resize(d.size());
  vcl_memcpy(data_,d.data_,size_*sizeof(T));
  return *this;
}
//: Increment reference count
template<class T>
void mil_image_data<T>::ref()
{
  ref_count_++;
}

//: Decrement reference count
template<class T>
void mil_image_data<T>::unref()
{
  ref_count_--;
  if (ref_count_==0)
  {
    delete [] data_; data_=0;
    delete this;
  }
}


//: Create space for n elements
template<class T>
void mil_image_data<T>::resize(int n)
{
  if (size_==(unsigned int)n) return;
  delete [] data_;
  data_ = 0;
  if (n>0)
    data_ = new T[n];
  size_ = n;
}

//: Version number for I/O
template<class T>
short mil_image_data<T>::version_no() const
{
  return 1;
}

//: Does the name of the class match the argument?
template<class T>
bool mil_image_data<T>::is_class(vcl_string const& s) const
{
  return s==mil_image_data<T>::is_a();
}

//: Print class to os
template<class T>
void mil_image_data<T>::print_summary(vcl_ostream& os) const
{
  os<<size_<<" elements.";
}

//: Save class to binary file stream
template<class T>
void mil_image_data<T>::b_write(vsl_b_ostream& bfs) const
{
  vsl_b_write(bfs,version_no());
  vsl_b_write(bfs,size_);
  for (unsigned i=0; i<size_; ++i)
    vsl_b_write(bfs, data_[i]);
}

//: Load class from binary stream
template<class T>
void mil_image_data<T>::b_read(vsl_b_istream& bfs)
{
  if (!bfs) return;

  unsigned int n;
  short v;
  vsl_b_read(bfs,v);
  switch (v)
  {
  case (1):
    vsl_b_read(bfs,n);
    resize(n);
    for (unsigned i=0; i<n; ++i)
      vsl_b_read(bfs, data_[i]);
    break;
  default:
    vcl_cerr << "I/O ERROR: mil_image_data<T>::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< v << '\n';
    bfs.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}


#if 0
// MSVC confuses the templated vsl_b_read(s, myclass<T> &)
// and vsl_b_read(s, myclass<T> *&). We don't really need the former

//: Write to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T>& v)
{
  v.b_write(s);
}

//: Read from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& s, mil_image_data<T>& v)
{
  v.b_read(s);
}
#endif

//: Write to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& os, const mil_image_data<T>* p)
{
  if (p==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
    //vsl_b_write(os,*p);
  }
}


//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& is, mil_image_data<T>* & v)
{
  delete v;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    v = new mil_image_data<T>();
    v->b_read(is);
    //vsl_b_read(is, *v);
  }
  else
    v = 0;
}


//: Print class to os
template<class T>
void vsl_print_summary(vcl_ostream& os, const mil_image_data<T>* p)
{
  p->print_summary(os);
}


#undef MIL_IMAGE_DATA_INSTANTIATE
#define MIL_IMAGE_DATA_INSTANTIATE(T) \
VCL_DEFINE_SPECIALIZATION vcl_string mil_image_data<T >::is_a() const \
{ return vcl_string("mil_image_data<" #T ">"); } \
template class mil_image_data<T >; \
template void vsl_b_write(vsl_b_ostream&s, const mil_image_data<T >*v); \
template void vsl_b_read(vsl_b_istream& s, mil_image_data<T >* & v); \
template void vsl_print_summary(vcl_ostream& s, const mil_image_data<T >* p)


#endif // mil_image_data_txx_
