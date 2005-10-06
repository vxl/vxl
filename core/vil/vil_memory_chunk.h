// This is core/vil/vil_memory_chunk.h
#ifndef vil_memory_chunk_h_
#define vil_memory_chunk_h_
//:
//  \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <vil/vil_smart_ptr.h>
#include <vil/vil_pixel_format.h>

//: Ref. counted block of data on the heap.
//  Image data block used by vil_image_view<T>.
class vil_memory_chunk
{
    //: Data
    void *data_;

    //: Number of elements (bytes)
    unsigned long size_;

    //: Indicate what format data is (used for binary IO)
    // Should always be a scalar type.
    vil_pixel_format pixel_format_;

    //: Reference count
    int ref_count_;

 public:
    //: Dflt ctor
    vil_memory_chunk();

    //: Allocate n bytes of memory
    // \param pixel_format indicates what format to be used for binary IO,
    // and should always be a scalar type.
    vil_memory_chunk(unsigned long n, vil_pixel_format pixel_format);

    //: Copy ctor
    vil_memory_chunk(const vil_memory_chunk&);

    //: Copy operator
    vil_memory_chunk& operator=(const vil_memory_chunk&);

    //: Destructor
    virtual ~vil_memory_chunk();

    //: Increment reference count
    void ref() { ref_count_++; }

    //: Decrement reference count
    void unref();

    //: Number of objects referring to this data
    int ref_count() const { return ref_count_; }

    //: Pointer to first element of data
    void* data() { return data_;}

    //: Pointer to first element of data
    void* const_data() const { return data_;}

    //: Indicate what format data is to be saved as in binary IO
    vil_pixel_format pixel_format() const { return pixel_format_; }

    //: Number of bytes allocated
    unsigned long size() const { return size_; }

    //: Create space for n bytes
    //  pixel_format indicates what format to be used for binary IO
    void set_size(unsigned long n, vil_pixel_format pixel_format);
};

typedef vil_smart_ptr<vil_memory_chunk> vil_memory_chunk_sptr;

#endif // vil_memory_chunk_h_
