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
#include <vil2/vil2_stream.h>

//: Reader/Writer for GIPL format images.
class vil3d_gipl_format : public vil3d_file_format
{
public:
  vil3d_gipl_format();
  //: The destructor must be virtual so that the memory chunk is destroyed.
  virtual ~vil3d_gipl_format();

  virtual vil3d_image_resource_sptr make_input_image(const char *) const;

  //: Make a "generic_image" on which put_section may be applied.
  // The file may bo openned immediately for writing so that a header can be written.
  virtual vil3d_image_resource_sptr make_output_image(const char* filename,
                                                     unsigned ni,
                                                     unsigned nj,
                                                     unsigned nk,
                                                     unsigned nplanes,
                                                     enum vil2_pixel_format)const;

  //: default filename tag for this image.
  virtual const char * tag() const {return "gipl";}


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
