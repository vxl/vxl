// This is brl/bbas/vidl2/vidl2_dc1394_istream.h
#ifndef vidl2_dc1394_istream_h_
#define vidl2_dc1394_istream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
// version 1 API.  Linking to both version of libdc1394 will result
// in linking errors.
//
// Currently this code works with libdc1394 version 2.0.0-pre7.
// The authors note that the libdc1394 API is subject to change in
// prerelease version.  As a result, the vidl2_dc1394_istream is
// is subject to change with it.

#include "vidl2_istream.h"
#include "vidl2_iidc1394_params.h"
#include <vcl_string.h>
#include <vil/vil_image_resource_sptr.h>


//: A video input stream using libdc1394
// This stream uses the dc1394 API (v2.0) to stream
// video from a IEEE 1394 camera
class vidl2_dc1394_istream
  : public vidl2_istream
{
 public:
  //: Constructor - default
  vidl2_dc1394_istream();

  //: Constructor - from a filename
  vidl2_dc1394_istream(const vcl_string& filename);

  //: Destructor
  virtual ~vidl2_dc1394_istream();

  //: Open a new stream using a filename
  virtual bool open(const vcl_string& device_filename = "/dev/video1394/0",
                    unsigned int num_dma_buffers = 2,
                    bool drop_frames = false,
                    const vidl2_iidc1394_params& params = vidl2_iidc1394_params());

  //: Close the stream
  virtual void close();

  //: Probe the bus to determine the valid parameter options
  static bool valid_params(vidl2_iidc1394_params::valid_options& options);


  //: Return true if the stream is open for reading
  virtual bool is_open() const;

  //: Return true if the stream is in a valid state
  virtual bool is_valid() const;

  //: Return true if the stream support seeking
  virtual bool is_seekable() const;

  //: Return the current frame number
  virtual unsigned int frame_number() const;

  //: Advance to the next frame (but don't acquire an image)
  virtual bool advance();

  //: Read the next frame from the stream (advance and acquire)
  virtual vidl2_frame_sptr read_frame();

  //: Return the current frame in the stream
  virtual vidl2_frame_sptr current_frame();

  //: Seek to the given frame number
  // \returns true if successful
  virtual bool seek_frame(unsigned int frame_number);

 private:
  //: The private implementation (PIMPL) details.
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* is_;
};

#endif // vidl2_dc1394_istream_h_
