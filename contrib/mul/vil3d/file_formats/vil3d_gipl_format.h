// This is mul/vil3d/file_formats/vil3d_gipl_format.h
#ifndef vil3d_gipl_format_h_
#define vil3d_gipl_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for GIPL format images.
// \author Tim Cootes - Manchester

#include <vil3d/vil3d_file_format.h>

//: Reader/Writer for GIPL format images.
class vil3d_gipl_format : public vil3d_file_format
{
 public:
  // The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_gipl_format() {};

    //: Read header and image from given stream if possible
  virtual bool read_stream(vil3d_header_data_sptr& header,
                           vil3d_image_view_base_sptr& image,
                           vil2_stream *is);

    //: Write header and image to given stream if possible
  virtual bool write_stream(const vil3d_header_data_sptr& header,
                            const vil3d_image_view_base_sptr& image,
                            vil2_stream *os);
};

#endif
