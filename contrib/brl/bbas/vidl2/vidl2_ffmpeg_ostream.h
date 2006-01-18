// This is brl/bbas/vidl2/vidl2_ffmpeg_ostream.h
#ifndef vidl2_ffmpeg_ostream_h_
#define vidl2_ffmpeg_ostream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video output stream to an encoded file using FFMPEG
//
// \author Matt Leotta
// \date 3 Jan 2006
//
// \verbatim
//  Modifications
//   Matt Leotta   3 Jan 2006   Adapted from code by Amitha Perera
// \endverbatim

#include "vidl2_ostream.h"
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>


//: The parameters used to initialize a ffmpeg writer.
struct vidl2_ffmpeg_ostream_params;


//: A video output stream to an encoded file using FFMPEG
class vidl2_ffmpeg_ostream
  : public vidl2_ostream
{
 public:
  //: Constructor
  vidl2_ffmpeg_ostream();

  //: Constructor - opens a stream
  vidl2_ffmpeg_ostream(const vcl_string& filename,
                       const vidl2_ffmpeg_ostream_params& params);

  //: Destructor
  virtual ~vidl2_ffmpeg_ostream();

  //: Open the stream
  virtual bool open(const vcl_string& filename,
                    const vidl2_ffmpeg_ostream_params& params);

  //: Close the stream
  virtual void close();

  //: Return true if the stream is open for writing
  virtual bool is_open() const;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vidl2_frame_sptr& frame);

 private:
  //: The private implementation (PIMPL) details
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* os_;
};

#endif // vidl2_ffmpeg_ostream_h_
