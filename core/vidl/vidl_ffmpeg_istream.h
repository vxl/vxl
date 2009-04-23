// This is core/vidl/vidl_ffmpeg_istream.h
#ifndef vidl_ffmpeg_istream_h_
#define vidl_ffmpeg_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video input stream using FFMPEG to decoded files
//
// \author Matt Leotta
// \date 21 Dec 2005
//
// \verbatim
//  Modifications
//    Matt Leotta   21 Dec 2005   Adapted from code by Amitha Perera
// \endverbatim

#include "vidl_istream.h"
#include <vcl_string.h>


//: A video input stream using FFMPEG to decoded files
class vidl_ffmpeg_istream
  : public vidl_istream
{
 public:
  //: Constructor - default
  vidl_ffmpeg_istream();

  //: Constructor - from a filename
  vidl_ffmpeg_istream(const vcl_string& filename);

  //: Destructor
  virtual ~vidl_ffmpeg_istream();

  //: Open a new stream using a filename
  virtual bool open(const vcl_string& filename);

  //: Close the stream
  virtual void close();

  //: Return true if the stream is open for reading
  virtual bool is_open() const;

  //: Return true if the stream is in a valid state
  virtual bool is_valid() const;

  //: Return true if the stream support seeking
  virtual bool is_seekable() const;

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  virtual int num_frames() const;

  //: Return the current frame number
  virtual unsigned int frame_number() const;

  //: Return the width of each frame
  virtual unsigned int width() const;

  //: Return the height of each frame
  virtual unsigned int height() const;

  //: Return the pixel format
  virtual vidl_pixel_format format() const;

  //: Return the frame rate (FPS, 0.0 if unspecified)
  virtual double frame_rate() const;
  
  //: Return the duration in seconds (0.0 if unknown)
  virtual double duration() const;

  //: Advance to the next frame (but don't acquire an image)
  virtual bool advance();

  //: Read the next frame from the stream (advance and acquire)
  virtual vidl_frame_sptr read_frame();

  //: Return the current frame in the stream
  virtual vidl_frame_sptr current_frame();

  //: Seek to the given frame number
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number);

 private:
  //: The private implementation (PIMPL) details.
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* is_;
};

#endif // vidl_ffmpeg_istream_h_
