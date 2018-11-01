// This is core/vidl/vidl_ostream.h
#ifndef vidl_ostream_h_
#define vidl_ostream_h_
//:
// \file
// \brief A base class for output video streams
//
// \author Matt Leotta
// \date 19 Dec 2005

#include "vidl_frame_sptr.h"


//: A base class for output video streams
class vidl_ostream
{
 public:
  //: Constructor
  vidl_ostream() : ref_count_(0) {}
  //: Destructor
  virtual ~vidl_ostream() = default;

  //: Close the stream
  virtual void close() = 0;

  //: Return true if the stream is open for writing
  virtual bool is_open() const = 0;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vidl_frame_sptr& frame) = 0;

 private:
  //: prevent deep copying a stream
  vidl_ostream(const vidl_ostream& /*other*/) : ref_count_(0) {}

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

#endif // vidl_ostream_h_
