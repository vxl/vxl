// This is core/vidl/vidl_ffmpeg_istream.h
#ifndef vidl_ffmpeg_istream_h_
#define vidl_ffmpeg_istream_h_
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

#include <string>
#include <vector>
#include <deque>
#include "vidl_istream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


//: A video input stream using FFMPEG to decoded files
class VIDL_EXPORT vidl_ffmpeg_istream
  : public vidl_istream
{
 public:
  //: Constructor - default
  vidl_ffmpeg_istream();

  //: Constructor - from a filename
  vidl_ffmpeg_istream(const std::string& filename);

  //: Destructor
  ~vidl_ffmpeg_istream() override;

  //: Open a new stream using a filename
  virtual bool open(const std::string& filename);

  //: Close the stream
  void close() override;

  //: Return true if the stream is open for reading
  bool is_open() const override;

  //: Return true if the stream is in a valid state
  bool is_valid() const override;

  //: Return true if the stream support seeking
  bool is_seekable() const override;

  //: Return the number of frames if known
  //  returns -1 for non-seekable streams
  int num_frames() const override;

  //: Return the current frame number
  unsigned int frame_number() const override;

  //: Return the width of each frame
  unsigned int width() const override;

  //: Return the height of each frame
  unsigned int height() const override;

  //: Return the pixel format
  vidl_pixel_format format() const override;

  //: Return the frame rate (FPS, 0.0 if unspecified)
  double frame_rate() const override;

  //: Return the duration in seconds (0.0 if unknown)
  double duration() const override;

  //: Advance to the next frame (but don't acquire an image)
  bool advance() override;

  //: Read the next frame from the stream (advance and acquire)
  vidl_frame_sptr read_frame() override;

  //: Return the current frame in the stream
  vidl_frame_sptr current_frame() override;

  //: Return the current presentation time stamp
  virtual double current_pts() const;

  //: Return the current video packet's data, is used to get
  //  video stream embeded metadata.
  virtual std::vector<vxl_byte> current_packet_data() const;

  //: Return the raw metadata bytes obtained while reading the current frame.
  //  This deque will be empty if there is no metadata stream
  //  Metadata is often encoded as KLV,
  //  but no attempt to decode KLV is made here
  std::deque<vxl_byte> current_metadata();

  //: Return true if the video also has a metadata stream
  bool has_metadata() const;

  //: Seek to the given frame number
  // \returns true if successful
  bool seek_frame(unsigned int frame_number) override;

 private:
  //: The private implementation (PIMPL) details.
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* is_;
};

#endif // vidl_ffmpeg_istream_h_
