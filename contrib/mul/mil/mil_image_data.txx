#ifndef mil_image_data_txx_
#define mil_image_data_txx_

//: \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <mil/mil_image_data.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

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
  memcpy(data_,d.data_,size_*sizeof(T));
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
    if (size_==n) return;
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

//: Name of the class
template<class T>
vcl_string mil_image_data<T>::is_a() const
{
    return vcl_string("mil_image_data<T>");
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
    vsl_b_write_block(bfs,data_,size_);
}

//: Load class from binary stream
template<class T>
void mil_image_data<T>::b_read(vsl_b_istream& bfs)
{
    unsigned int n;
    short v;
    vsl_b_read(bfs,v);
    switch (v)
    {
    case (1):
        vsl_b_read(bfs,n);
        resize(n);
        vsl_b_read_block(bfs,data_,size_);
        break;
    default:
        vcl_cerr<<"mil_image_data<T>::b_read()"
            <<" Unexpected version number "<<v<<vcl_endl;
        vcl_abort();
    }
}


//: Write  to binary stream
template<class T>
void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T>& v)
{
    v.b_write(s);
}

//: Read  from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& s, mil_image_data<T>& v)
{
    v.b_read(s);
}

//: Write  to binary stream
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
    vsl_b_write(os,*p);
  }
}

//: Read data from binary stream
template<class T>
void vsl_b_read(vsl_b_istream& is, mil_image_data<T>*& v)
{
  delete v;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    v = new mil_image_data<T>();
    vsl_b_read(is, *v);
  }
  else
    v = 0;
}


#if 0 // For reasons I don't understand the following causes an error
#undef MIL_IMAGE_DATA_INSTANTIATE
#define MIL_IMAGE_DATA_INSTANTIATE(T) \
template class mil_image_data<T >; \
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T >& v); \
template void vsl_b_read(vsl_b_istream& s, mil_image_data<T >& v); \
template void vsl_b_write(vsl_b_ostream& s, const mil_image_data<T >* v); \
template void vsl_b_read(vsl_b_istream& s, mil_image_data<T >*& v)
#endif // 0


#endif // mil_image_data_txx_
