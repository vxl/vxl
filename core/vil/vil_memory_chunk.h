// This is mul/vil2/vil2_memory_chunk.h
#ifndef vil2_memory_chunk_h_
#define vil2_memory_chunk_h_
//:
//  \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <vil2/vil2_smart_ptr.h>
#include <vil2/vil2_pixel_format.h>

//: Ref. counted block of data on the heap.
//  Image data block used by vil2_image_view<T>.
class vil2_memory_chunk
{
    //: Data
    void *data_;

    //: Number of elements (bytes)
    unsigned long size_;

    //: Indicate what format data is (used for binary IO)
    vil2_pixel_format pixel_format_;

    //: Reference count
    int ref_count_;

 public:
    //: Dflt ctor
    vil2_memory_chunk();

    //: Allocate n bytes of memory
    //  pixel_format indicates what format to be used for binary IO
    vil2_memory_chunk(unsigned long n, vil2_pixel_format pixel_format);

    //: Copy ctor
    vil2_memory_chunk(const vil2_memory_chunk&);

    //: Copy operator
    vil2_memory_chunk& operator=(const vil2_memory_chunk&);

    //: Destructor
    virtual ~vil2_memory_chunk();

    //: Increment reference count
    void ref() { ref_count_++; }

    //: Decrement reference count
    void unref();

    //: Number of objects refering to this data
    int ref_count() const { return ref_count_; }

    //: Pointer to first element of data
    void* data() { return data_;}

    //: Pointer to first element of data
    void* const_data() const { return data_;}

    //: Indicate what format data is to be saved as in binary IO
    vil2_pixel_format pixel_format() const { return pixel_format_; }

    //: Number of bytes allocated
    unsigned long size() const { return size_; }

    //: Create space for n elements
    //  pixel_format indicates what format to be used for binary IO
    void resize(unsigned long n, vil2_pixel_format pixel_format);
};

typedef vil2_smart_ptr<vil2_memory_chunk> vil2_memory_chunk_sptr;

#endif // vil2_memory_chunk_h_
