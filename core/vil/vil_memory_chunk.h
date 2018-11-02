// This is core/vil/vil_memory_chunk.h
#ifndef vil_memory_chunk_h_
#define vil_memory_chunk_h_
//:
//  \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

#include <cstddef>
#include <vcl_atomic_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_smart_ptr.h>
#include <vil/vil_pixel_format.h>

//: Ref. counted block of data on the heap.
//  Image data block used by vil_image_view<T>.
class vil_memory_chunk
{
  protected:
    //: Data
    void *data_;

    //: Number of elements (bytes)
    std::size_t size_;

    //: Indicate what format data is (used for binary IO)
    // Should always be a scalar type.
    vil_pixel_format pixel_format_;

    //: Reference count
    vcl_atomic_count ref_count_;

 public:
    //: Dflt ctor
    vil_memory_chunk();

    //: Allocate n bytes of memory
    // \param pixel_format indicates what format to be used for binary IO,
    // and should always be a scalar type.
    vil_memory_chunk(std::size_t n, vil_pixel_format pixel_format);

    //: Copy ctor
    vil_memory_chunk(const vil_memory_chunk&);

    //: Copy operator
    vil_memory_chunk& operator=(const vil_memory_chunk&);

    //: Destructor
    virtual ~vil_memory_chunk();

    //: Increment reference count
    void ref() { ++ref_count_; }

    //: Decrement reference count
    void unref();

    //: Number of objects referring to this data
    long ref_count() const { return ref_count_; }

    //: Pointer to first element of data
    virtual void* data();

    //: Pointer to first element of data
    virtual void* const_data() const;

    //: Indicate what format data is to be saved as in binary IO
    vil_pixel_format pixel_format() const { return pixel_format_; }

    //: Number of bytes allocated
    std::size_t size() const { return size_; }

    //: Create space for n bytes
    //  pixel_format indicates what format to be used for binary IO
    virtual void set_size(unsigned long n, vil_pixel_format pixel_format);
};

typedef vil_smart_ptr<vil_memory_chunk> vil_memory_chunk_sptr;

#endif // vil_memory_chunk_h_
