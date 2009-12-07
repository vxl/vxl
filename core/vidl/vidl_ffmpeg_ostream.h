// This is core/vidl/vidl_ffmpeg_ostream.h
#ifndef vidl_ffmpeg_ostream_h_
#define vidl_ffmpeg_ostream_h_
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

#include "vidl_ostream.h"
#include <vcl_string.h>
#include "vidl_ffmpeg_ostream_params.h"


//: The parameters used to initialize a ffmpeg writer.
struct vidl_ffmpeg_ostream_params;


//: A video output stream to an encoded file using FFMPEG
class vidl_ffmpeg_ostream
  : public vidl_ostream
{
 public:
  //: Constructor
  vidl_ffmpeg_ostream();

  //: Constructor - opens a stream
  vidl_ffmpeg_ostream(const vcl_string& filenam,
                      const vidl_ffmpeg_ostream_params& parms);

  //: Destructor
  virtual ~vidl_ffmpeg_ostream();

  //: Open the stream
  virtual bool open();

  //: Close the stream
  virtual void close();

  //: Return true if the stream is open for writing
  virtual bool is_open() const;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vidl_frame_sptr& frame);

  //: Set the filename
  void set_filename(const vcl_string& filenam) { filename_ = filenam; }

  //: Set the parameters
  void set_params(const vidl_ffmpeg_ostream_params& parms) { params_ = parms; }

  //: Access the filename
  vcl_string filename() const { return filename_; }

  //: Access the parameters
  const vidl_ffmpeg_ostream_params& params() const { return params_; }

 private:
  //: The private implementation (PIMPL) details
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* os_;

  //: The filename to open
  vcl_string filename_;

  //: The parameters
  vidl_ffmpeg_ostream_params params_;
};

#endif // vidl_ffmpeg_ostream_h_
