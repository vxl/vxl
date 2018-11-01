// This is core/vidl/vidl_frame.h
#ifndef vidl_frame_h_
#define vidl_frame_h_
//:
// \file
// \brief A ref counted video frame
//
// \author Matt Leotta
// \date 13 Jan 2006
//
//  The vidl_frame is meant to be an object for transmitting a
//  frame buffer from a vidl_istream to a vidl_ostream.  It
//  is possible that the actual image buffer cannot be wrapped
//  by a vil_image_view in a meaningful way (i.e. YUV 4:2:2).
//  To work with a frame as an image use vidl_convert_to_view

#include "vidl_pixel_format.h"
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view_base.h>


//: A ref counted video frame
class VIDL_EXPORT vidl_frame
{
  public:
    //: Destructor
    virtual ~vidl_frame() = default;

    //: Make the buffer invalid (data()==0 and size()==0)
    virtual void invalidate() { ni_=0; nj_=0; format_=VIDL_PIXEL_FORMAT_UNKNOWN; }

    //: Return a pointer to the first element of data
    virtual void * data() = 0;
    virtual const void * data() const = 0;

    //: The size of the buffer in bytes
    virtual unsigned long size() const = 0;

    //: Width
    unsigned ni() const { return ni_; }

    //: Height
    unsigned nj() const { return nj_; }

    //: Return the pixel format
    vidl_pixel_format pixel_format() const { return format_; }

  protected:
    //: Constructor
    vidl_frame():
      ni_(0), nj_(0), format_(VIDL_PIXEL_FORMAT_UNKNOWN), ref_count_(0) {}

    //: Constructor
    vidl_frame(unsigned ni, unsigned nj, vidl_pixel_format fmt):
      ni_(ni), nj_(nj), format_(fmt), ref_count_(0) {}

    //: frame width
    unsigned ni_;
    //: frame height
    unsigned nj_;
    //: frame pixel format
    vidl_pixel_format format_;

  //-------------------------------------------------------
  // reference counting
  public:

    //: Increment reference count
    void ref() { ref_count_++; }

    //: Decrement reference count
    void unref();

    //: Number of objects referring to this data
    int ref_count() const { return ref_count_; }

  private:
    int ref_count_;
};


//: A frame buffer that shares someone else's data
class VIDL_EXPORT vidl_shared_frame : public vidl_frame
{
  public:
    //: Constructor
    vidl_shared_frame():
      vidl_frame(), buffer_(nullptr) {}

    //: Constructor
    vidl_shared_frame(void * buffer, unsigned ni, unsigned nj, vidl_pixel_format fmt):
      vidl_frame(ni,nj,fmt), buffer_(buffer) {}

    //: Destructor
    ~vidl_shared_frame() override = default;

    //: Make the buffer invalid (data()==0 and size()==0)
    void invalidate() override { buffer_ = nullptr; vidl_frame::invalidate(); }

    //: Return a pointer to the first element of data
    void * data() override { return buffer_; }
    const void * data() const override { return buffer_; }

    //: The size of the buffer in bytes
    unsigned long size() const override { return vidl_pixel_format_buffer_size(ni_,nj_,format_); }

  private:
    void * buffer_;
};


//: A frame buffer that wraps a vil_memory_chunk
//  This is useful when the frame actually came from a vil_image
class VIDL_EXPORT vidl_memory_chunk_frame : public vidl_frame
{
  public:
    //: Constructor
    vidl_memory_chunk_frame() : memory_(nullptr) {}

    //: Constructor - from a vil_memory_chunk_sptr
    vidl_memory_chunk_frame(unsigned ni, unsigned nj, vidl_pixel_format fmt,
                            const vil_memory_chunk_sptr& memory):
      vidl_frame(ni,nj,fmt), memory_(memory) {}

    //: Constructor - from a vil_image_view
    // return an invalid frame if the image format can not be wrapped
    // \param fmt if not UNKNOWN, requires this pixel or fails
    vidl_memory_chunk_frame(const vil_image_view_base& image,
                            vidl_pixel_format fmt = VIDL_PIXEL_FORMAT_UNKNOWN);

    //: Return the memory chunk
    // used in recreating a vil_image_view
    inline const vil_memory_chunk_sptr& memory_chunk() const { return memory_; }

    //: Destructor
    ~vidl_memory_chunk_frame() override = default;

    //: Make the buffer invalid (data()==0 and size()==0)
    void invalidate() override { memory_ = nullptr;  vidl_frame::invalidate(); }

    //: Return a pointer to the first element of data
    void * data () override { return memory_?memory_->data():nullptr; }
    const void * data () const override { return memory_?memory_->data():nullptr; }

    //: The size of the buffer in bytes
    unsigned long size() const override { return (unsigned long)(memory_?memory_->size():0L); }

  private:
    vil_memory_chunk_sptr memory_;
};


#endif // vidl_frame_h_
