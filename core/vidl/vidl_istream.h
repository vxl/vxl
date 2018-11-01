// This is core/vidl/vidl_istream.h
#ifndef vidl_istream_h_
#define vidl_istream_h_
//:
// \file
// \brief A base class for input video streams
//
// \author Matt Leotta
// \date 19 Dec 2005

#include "vidl_frame_sptr.h"
#include "vidl_pixel_format.h"

//: A base class for input video streams
class vidl_istream
{
 public:
  //: Constructor
  vidl_istream() : ref_count_(0) {}
  //: Destructor
  virtual ~vidl_istream() = default;

  //: Return true if the stream is open for reading
  virtual bool is_open() const = 0;

  //: Return true if the stream is in a valid state
  // Streams open in an invalid state pointing to the frame
  // before the first valid frame.  The stream becomes valid when
  // it is first advanced.
  virtual bool is_valid() const = 0;

  //: Return true if the stream supports seeking
  virtual bool is_seekable() const = 0;

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  virtual int num_frames() const = 0;

  //: Return the current frame number
  //  before the first call to advance() the frame number
  //  is static_cast<unsigned int>(-1)
  virtual unsigned int frame_number() const = 0;

  //: Return the width of each frame
  virtual unsigned int width() const = 0;

  //: Return the height of each frame
  virtual unsigned int height() const = 0;

  //: Return the pixel format
  virtual vidl_pixel_format format() const = 0;

  //: Return the frame rate (FPS, 0.0 if unspecified)
  virtual double frame_rate() const = 0;

  //: Return the duration in seconds (0.0 if unknown)
  virtual double duration() const = 0;

  //: Close the stream
  virtual void close() = 0;

  //: Advance to the next frame (but don't acquire an image)
  virtual bool advance() = 0 ;

  //: Read the next frame from the stream (advance and acquire)
  virtual vidl_frame_sptr read_frame() = 0;

  //: Return the current frame in the stream
  virtual vidl_frame_sptr current_frame() = 0;

  //: Seek to the given frame number
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number) = 0;

 private:
  //: prevent deep copying a stream
  vidl_istream(const vidl_istream& /*other*/) : ref_count_(0) {}

 //-------------------------------------------------------
 // reference counting
 public:

  //: Increment reference count
  void ref() { ref_count_++; }

  //: Decrement reference count
  void unref() { if (--ref_count_ <= 0) delete this; }

  //: Number of objects referring to this data
  int ref_count() const { return ref_count_; }

 private:
  int ref_count_;
};

#endif // vidl_istream_h_
