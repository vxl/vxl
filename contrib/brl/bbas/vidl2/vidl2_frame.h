// This is brl/bbas/vidl2/vidl2_frame.h
#ifndef vidl2_frame_h_
#define vidl2_frame_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A ref counted video frame
//
// \author Matt Leotta
// \date 13 Jan 2006
//
//  The vidl2_frame is meant to be an object for transmitting a
//  frame buffer from a vidl2_istream to a vidl2_ostream.  It
//  is possible that the actual image buffer cannot be wrapped
//  by a vil_image_view in a meaningful way (i.e. YUV 4:2:2).
//  To work with a frame as an image use vidl2_convert_to_view

#include "vidl2_pixel_format.h"
#include "vidl2_frame_sptr.h"
#include <vil/vil_memory_chunk.h>
#include <vil/vil_image_view_base.h>


//: A ref counted video frame
class vidl2_frame
{
  public:
    //: Destructor
    virtual ~vidl2_frame() {}

    //: Make the buffer invalid (data()==0 and size()==0)
    virtual void invalidate() { ni_=0; nj_=0; format_=VIDL2_PIXEL_FORMAT_UNKNOWN; }

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
    vidl2_pixel_format pixel_format() const { return format_; }

  protected:
    //: Constructor
    vidl2_frame():
      ni_(0), nj_(0), format_(VIDL2_PIXEL_FORMAT_UNKNOWN), ref_count_(0) {}

    //: Constructor
    vidl2_frame(unsigned ni, unsigned nj, vidl2_pixel_format fmt):
      ni_(ni), nj_(nj), format_(fmt), ref_count_(0) {}

    //: frame width
    unsigned ni_;
    //: frame height
    unsigned nj_;
    //: frame pixel format
    vidl2_pixel_format format_;

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
class vidl2_shared_frame : public vidl2_frame
{
  public:
    //: Constructor
    vidl2_shared_frame():
      vidl2_frame(), buffer_(NULL) {}

    //: Constructor
    vidl2_shared_frame(void * buffer, unsigned ni, unsigned nj, vidl2_pixel_format fmt):
      vidl2_frame(ni,nj,fmt), buffer_(buffer) {}

    //: Destructor
    virtual ~vidl2_shared_frame() {}

    //: Make the buffer invalid (data()==0 and size()==0)
    virtual void invalidate() { buffer_ = 0; vidl2_frame::invalidate(); }

    //: Return a pointer to the first element of data
    virtual void * data() { return buffer_; }
    virtual const void * data() const { return buffer_; }

    //: The size of the buffer in bytes
    virtual unsigned long size() const { return vidl2_pixel_format_buffer_size(ni_,nj_,format_); }

  private:
    void * buffer_;
};


//: A frame buffer that wraps a vil_memory_chunk
//  This is useful when the frame actually came from a vil_image
class vidl2_memory_chunk_frame : public vidl2_frame
{
  public:
    //: Constructor
    vidl2_memory_chunk_frame() : memory_(NULL) {}

    //: Constructor - from a vil_memory_chunk_sptr
    vidl2_memory_chunk_frame(unsigned ni, unsigned nj, vidl2_pixel_format fmt,
                             const vil_memory_chunk_sptr& memory):
      vidl2_frame(ni,nj,fmt), memory_(memory) {}

    //: Constructor - from a vil_image_view
    // return an invalid frame if the image format can not be wrapped
    // \param fmt if not UNKNOWN, requires this pixel or fails
    vidl2_memory_chunk_frame(const vil_image_view_base& image,
                             vidl2_pixel_format fmt = VIDL2_PIXEL_FORMAT_UNKNOWN);

    //: Return the memory chunk
    // used in recreating a vil_image_view
    inline const vil_memory_chunk_sptr& memory_chunk() const { return memory_; }

    //: Destructor
    virtual ~vidl2_memory_chunk_frame() {}

    //: Make the buffer invalid (data()==0 and size()==0)
    virtual void invalidate() { memory_ = NULL;  vidl2_frame::invalidate(); }

    //: Return a pointer to the first element of data
    virtual void * data () { return memory_?memory_->data():NULL; }
    virtual const void * data () const { return memory_?memory_->data():NULL; }

    //: The size of the buffer in bytes
    virtual unsigned long size() const { return memory_?memory_->size():0; }

  private:
    vil_memory_chunk_sptr memory_;
};


#endif // vidl2_frame_h_
