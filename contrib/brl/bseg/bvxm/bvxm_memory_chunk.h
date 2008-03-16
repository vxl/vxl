#ifndef bvxm_memory_chunk_h_
#define bvxm_memory_chunk_h_
//:
// \file

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class bvxm_memory_chunk : public vbl_ref_count
{
  //: Data
  void *data_;

  //: Number of elements (bytes)
  unsigned long size_;

 public:
    //: Default constructor
    bvxm_memory_chunk();

    //: Allocate n bytes of memory
    bvxm_memory_chunk(unsigned long n);

    //: Copy constructor
    bvxm_memory_chunk(const bvxm_memory_chunk&);

    //: Copy operator
    bvxm_memory_chunk& operator=(const bvxm_memory_chunk&);

    //: Destructor
    virtual ~bvxm_memory_chunk();

    //: Pointer to first element of data
    void* data() { return data_;}

    //: Pointer to first element of data
    void* const_data() const { return data_;}

    //: Number of bytes allocated
    unsigned long size() const { return size_; }

    //: Create space for n bytes
    void set_size(unsigned long n);
};

typedef vbl_smart_ptr<bvxm_memory_chunk> bvxm_memory_chunk_sptr;

#endif

