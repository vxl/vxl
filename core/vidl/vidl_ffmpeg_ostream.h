// This is core/vidl/vidl_ffmpeg_ostream.h
#ifndef vidl_ffmpeg_ostream_h_
#define vidl_ffmpeg_ostream_h_
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

#include <string>
#include "vidl_ostream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "vidl_ffmpeg_ostream_params.h"


//: The parameters used to initialize a ffmpeg writer.
struct vidl_ffmpeg_ostream_params;


//: A video output stream to an encoded file using FFMPEG
class VIDL_EXPORT vidl_ffmpeg_ostream
  : public vidl_ostream
{
 public:
  //: Constructor
  vidl_ffmpeg_ostream();

  //: Constructor - opens a stream
  vidl_ffmpeg_ostream(const std::string  & filenam,
                      const vidl_ffmpeg_ostream_params  & parms);

  //: Destructor
  ~vidl_ffmpeg_ostream() override;

  //: Open the stream
  virtual bool open();

  //: Close the stream
  void close() override;

  //: Return true if the stream is open for writing
  bool is_open() const override;

  //: Write and image to the stream
  // \retval false if the image could not be written
  bool write_frame(const vidl_frame_sptr& frame) override;

  //: Set the filename
  void set_filename(const std::string& filenam) { filename_ = filenam; }

  //: Set the parameters
  void set_params(const vidl_ffmpeg_ostream_params& parms) { params_ = parms; }

  //: Access the filename
  std::string filename() const { return filename_; }

  //: Access the parameters
  const vidl_ffmpeg_ostream_params& params() const { return params_; }

 private:
  //: The private implementation (PIMPL) details
  //  This isolates the clients from the ffmpeg details
  struct pimpl;
  pimpl* os_;

  //: The filename to open
  std::string filename_;

  //: The parameters
  vidl_ffmpeg_ostream_params params_;
};

#endif // vidl_ffmpeg_ostream_h_
