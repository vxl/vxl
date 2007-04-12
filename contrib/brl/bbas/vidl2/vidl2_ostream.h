// This is brl/bbas/vidl2/vidl2_ostream.h
#ifndef vidl2_ostream_h_
#define vidl2_ostream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A base class for output video streams
//
// \author Matt Leotta
// \date 19 Dec 2005

#include "vidl2_frame_sptr.h"


//: A base class for output video streams
class vidl2_ostream
{
 public:
  //: Constructor
  vidl2_ostream() : ref_count_(0) {}
  //: Destructor
  virtual ~vidl2_ostream() {}

  //: Close the stream
  virtual void close() = 0;

  //: Return true if the stream is open for writing
  virtual bool is_open() const = 0;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vidl2_frame_sptr& frame) = 0;

   private:
  //: prevent deep copying a stream
  vidl2_ostream(const vidl2_ostream& other):ref_count_(0){}

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

#endif // vidl2_ostream_h_
