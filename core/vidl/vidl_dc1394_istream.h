// This is core/vidl/vidl_dc1394_istream.h
#ifndef vidl_dc1394_istream_h_
#define vidl_dc1394_istream_h_
//:
// \file
// \brief A video input stream using libdc1394
//
// \author Matt Leotta
// \date 6 Jan 2006
//
// \b WARNING this stream requires version 2 of the libdc1394 API.
// if you are also using the ffmpeg streams make sure your libavcodec
// and libavformat library are compiled without dc1394 support.
// ffmpeg supplies limited 1394 support through the libdc1394
// version 1 API.  Linking to both versions of libdc1394 will result
// in linking errors.
//
// Currently this code works with libdc1394 version 2.0.0-rc9.
// The authors note that the libdc1394 API is subject to change in
// prerelease version.  As a result, the vidl_dc1394_istream is
// is subject to change with it.

#include <string>
#include "vidl_istream.h"
#include "vidl_iidc1394_params.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: A video input stream using libdc1394
// This stream uses the dc1394 API (v2.0) to stream
// video from an IEEE 1394 camera
class vidl_dc1394_istream
  : public vidl_istream
{
 public:
  //: Constructor - default
  vidl_dc1394_istream();

  //: Destructor
  virtual ~vidl_dc1394_istream();

  //: Open a new stream using a filename
  virtual bool open(unsigned int num_dma_buffers = 2,
                    bool drop_frames = false,
                    const vidl_iidc1394_params& params = vidl_iidc1394_params());

  //: Close the stream
  virtual void close();

  //: Probe the bus to determine the valid parameter options
  static bool valid_params(vidl_iidc1394_params::valid_options& options);


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

#endif // vidl_dc1394_istream_h_
