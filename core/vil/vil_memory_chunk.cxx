//: \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <vil2/vil2_memory_chunk.h>
#include <vcl_cstring.h>

//: Dflt ctor
vil2_memory_chunk::vil2_memory_chunk()
: data_(0), size_(0), ref_count_(0)
{
}

//: Allocate n bytes of memory
vil2_memory_chunk::vil2_memory_chunk(unsigned n)
: data_(0), size_(0), ref_count_(0)
{
  resize(n);
}

//: Destructor
vil2_memory_chunk::~vil2_memory_chunk()
{
  delete [] (char*) data_;
}

//: Copy ctor
vil2_memory_chunk::vil2_memory_chunk(const vil2_memory_chunk& d)
: data_(0), size_(0), ref_count_(0)
{
  *this=d;
}

//: Copy operator
vil2_memory_chunk& vil2_memory_chunk::operator=(const vil2_memory_chunk& d)
{
  if (this==&d) return *this;
  
  resize(d.size());
  vcl_memcpy(data_,d.data_,size_);
  return *this;
}

//: Decrement reference count
void vil2_memory_chunk::unref()
{
  ref_count_--;
  if (ref_count_==0)
  {
    delete [] (char*) data_; data_=0;
    delete this;
  }
}

//: Create space for n elements
void vil2_memory_chunk::resize(int n)
{
  if (size_==(unsigned int)n) return;
  delete [] (char*) data_;
  data_ = 0;
  if (n>0)
    data_ = new char[n];
  size_ = n;
}


