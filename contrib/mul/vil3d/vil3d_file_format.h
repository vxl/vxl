// This is mul/vil3d/vil3d_file_format.h
#ifndef vil3d_file_format_h_
#define vil3d_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Base for objects capable of reading/writing different image formats.
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_header_data.h>
#include <vil3d/vil3d_image_view_base.h>
#include <vil2/vil2_stream.h>

//: Base for objects capable of reading/writing different image formats.
//
//  Design note: This is somewhat experimental at present.  A somewhat
//  simpler design is used compared to vil2, to get us started.  It
//  may be necessary to go to the more complex vil2 approach if the
//  following is not sufficiently flexible.
class vil3d_file_format
{
 public:
  // The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_file_format() {};

  //: Read header and image from given stream if possible
  virtual bool read_stream(vil3d_header_data_sptr& header,
                           vil3d_image_view_base_sptr& image,
                           vil2_stream *is)=0;

  //: Read header and image from named file if possible
  virtual bool read_file(vil3d_header_data_sptr& header,
                         vil3d_image_view_base_sptr& image,
                         const vcl_string& path);

  //: Write header and image to given stream if possible
  virtual bool write_stream(const vil3d_header_data_sptr& header,
                            const vil3d_image_view_base_sptr& image,
                            vil2_stream *os)=0;

  //: Write header and image to named file if possible
  virtual bool write_file(vil3d_header_data_sptr& header,
                          vil3d_image_view_base_sptr& image,
                          const vcl_string& path);

  //: Add a format reader to current list of those available
  static void add_format(vil3d_file_format* new_format);

  //: Number of formats available (number added by add_format()
  static unsigned n_formats();

  //: Access to available format readers supplied by add_format
  static vil3d_file_format** format_list();
};

#endif
