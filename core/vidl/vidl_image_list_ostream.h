// This is core/vidl/vidl_image_list_ostream.h
#ifndef vidl_image_list_ostream_h_
#define vidl_image_list_ostream_h_
//:
// \file
// \brief A video output stream to a list of images
//
// \author Matt Leotta
// \date 19 Dec 2005

#include <string>
#include "vidl_ostream.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:A video output stream to a list of images
class VIDL_EXPORT vidl_image_list_ostream
  : public vidl_ostream
{
 public:
  //: Constructor
  vidl_image_list_ostream();

  //: Constructor - opens a stream
  vidl_image_list_ostream(const std::string& directory,
                          const std::string& name_format = "%05d",
                          const std::string& file_format = "tiff",
                          const unsigned int init_index = 0);

  //: Destructor
  ~vidl_image_list_ostream() override { close(); }

  //: Open the stream
  virtual bool open(const std::string& directory,
                    const std::string& name_format = "%05d",
                    const std::string& file_format = "tiff",
                    const unsigned int init_index = 0);

  //: Close the stream
  void close() override;

  //: Return true if the stream is open for writing
  bool is_open() const override;

  //: Return the current index
  unsigned int index() const { return index_; }

  //: Return the next file name to be written to
  virtual std::string next_file_name() const;

  //: Write and image to the stream
  // \retval false if the image could not be written
  bool write_frame(const vidl_frame_sptr& frame) override;

 private:
  //: The current index
  unsigned int index_;

  //: The directory to save images in
  std::string dir_;

  //: The printf-style format string for filenames
  std::string name_format_;

  //: The image file format to use
  std::string file_format_;
};

#endif // vidl_image_list_ostream_h_
