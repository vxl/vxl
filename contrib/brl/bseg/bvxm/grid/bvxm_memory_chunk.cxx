#include "bvxm_memory_chunk.h"
//:
// \file
// \brief Ref. counted block of data on the heap. Based on vil_memory_chunk.cxx
// \author Dan Crispell

#include <vcl_cstring.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vbl/vbl_smart_ptr.txx>

//: Default constructor
bvxm_memory_chunk::bvxm_memory_chunk() : data_(0), size_(0) {}

//: Allocate n bytes of memory
bvxm_memory_chunk::bvxm_memory_chunk(vxl_uint_64 n)
{
  data_ = new (std::nothrow)char[(unsigned)n];
  
  if (data_ == 0) {
    vcl_cout << "bvxm_memory_chunk: Could not allocate data!" << vcl_endl;
  }
  
  size_ = n;
}

//: Destructor
bvxm_memory_chunk::~bvxm_memory_chunk()
{
  delete [] reinterpret_cast<char*>(data_);
}

//: Copy constructor
bvxm_memory_chunk::bvxm_memory_chunk(const bvxm_memory_chunk& d)
: vbl_ref_count(),
  data_(new char[(unsigned)d.size()]), size_(d.size())
{
  vcl_memcpy(data_,d.data_,(vcl_size_t)size_);
}

//: Assignment operator
bvxm_memory_chunk& bvxm_memory_chunk::operator=(const bvxm_memory_chunk& d)
{
  if (this==&d) return *this;

  set_size(d.size());
  vcl_memcpy(data_,d.data_,(vcl_size_t)size_);
  return *this;
}

//: Create empty space for n elements.
//  Leave existing data untouched if the size is already n.
void bvxm_memory_chunk::set_size(vxl_uint_64 n)
{
  if (size_==n) return;
  delete [] reinterpret_cast<char*>(data_);
  data_ = 0;
  if (n>0)
    data_ = new char[(unsigned)n];
  size_ = n;
}

VBL_SMART_PTR_INSTANTIATE(bvxm_memory_chunk);

