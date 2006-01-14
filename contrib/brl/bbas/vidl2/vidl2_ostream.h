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

#include <vil/vil_image_resource_sptr.h>


//: A base class for output video streams
class vidl2_ostream
{
 public:
  //: Constructor
  vidl2_ostream() {}
  //: Destructor
  virtual ~vidl2_ostream() {}

  //: Close the stream
  virtual void close() = 0;

  //: Return true if the stream is open for writing
  virtual bool is_open() const = 0;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vil_image_resource_sptr& image) = 0;
};

#endif // vidl2_ostream_h_
