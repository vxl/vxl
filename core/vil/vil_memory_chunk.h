// This is mul/vil2/vil2_memory_chunk.h
#ifndef vil2_memory_chunk_h_
#define vil2_memory_chunk_h_
//:
//  \file
//  \brief Ref. counted block of data on the heap
//  \author Tim Cootes

//: Ref. counted block of data on the heap
//  Image data block used by vil2_image_view<T>.
class vil2_memory_chunk
{
    //: Data
    void *data_;

    //: Number of elements
    unsigned int size_;

    //: Reference count
    int ref_count_;

 public:
    //: Dflt ctor
    vil2_memory_chunk();

    //: Allocate n bytes of memory
    vil2_memory_chunk(unsigned n);

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

    //: Number of bytes allocated
    unsigned int size() const { return size_; }

    //: Create space for n elements
    void resize(int n);
};

#endif

