// This is core/vidl/vidl_image_list_ostream.h
#ifndef vidl_image_list_ostream_h_
#define vidl_image_list_ostream_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A video output stream to a list of images
//
// \author Matt Leotta
// \date 19 Dec 2005

#include "vidl_ostream.h"
#include <vcl_string.h>

//:A video output stream to a list of images
class vidl_image_list_ostream
  : public vidl_ostream
{
 public:
  //: Constructor
  vidl_image_list_ostream();

  //: Constructor - opens a stream
  vidl_image_list_ostream(const vcl_string& directory,
                          const vcl_string& name_format = "%05d",
                          const vcl_string& file_format = "tiff",
                          const unsigned int init_index = 0);

  //: Destructor
  virtual ~vidl_image_list_ostream() { close(); }

  //: Open the stream
  virtual bool open(const vcl_string& directory,
                    const vcl_string& name_format = "%05d",
                    const vcl_string& file_format = "tiff",
                    const unsigned int init_index = 0);

  //: Close the stream
  virtual void close();

  //: Return true if the stream is open for writing
  virtual bool is_open() const;

  //: Return the current index
  unsigned int index() const { return index_; }

  //: Return the next file name to be written to
  virtual vcl_string next_file_name() const;

  //: Write and image to the stream
  // \retval false if the image could not be written
  virtual bool write_frame(const vidl_frame_sptr& frame);

 private:
  //: The current index
  unsigned int index_;

  //: The directory to save images in
  vcl_string dir_;

  //: The printf-style format string for filenames
  vcl_string name_format_;

  //: The image file format to use
  vcl_string file_format_;
};

#endif // vidl_image_list_ostream_h_
