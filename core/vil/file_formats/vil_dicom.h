// This is core/vil2/file_formats/vil2_dicom.h
#ifndef vil2_dicom_file_format_h_
#define vil2_dicom_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for DICOM format 2D images.
// \author Chris Wolstenholme, Ian Scott - Manchester

#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_file_format.h>
#include <vil2/file_formats/vil2_dicom_header.h>

class vil2_image_view_base;


//: Loader for DICOM files
class vil2_dicom_file_format : public vil2_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil2_image_resource_sptr make_input_image(vil2_stream* vs);
  virtual vil2_image_resource_sptr make_output_image(vil2_stream* vs,
                                                     unsigned ni,
                                                     unsigned nj,
                                                     unsigned nplanes,
                                                     vil2_pixel_format format);
};


//: Generic image implementation for DICOM files
class vil2_dicom_image : public vil2_image_resource
{
  vil2_stream* vs_;
  vil2_dicom_header_info header_;

  friend class vil2_dicom_file_format;


  vil2_streampos start_of_pixels_;

 public:
  vil2_dicom_image(vil2_stream* is, unsigned ni,
                   unsigned nj, unsigned nplanes,
                   vil2_pixel_format format);
  vil2_dicom_image(vil2_stream* is);
  ~vil2_dicom_image();

  // Inherit the documentation from vil2_image_resource

  virtual unsigned nplanes() const {return 1;}
  virtual unsigned ni() const {return header_.dimx_;}
  virtual unsigned nj() const {return header_.dimy_;}

  virtual enum vil2_pixel_format pixel_format() const;

  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
};

#endif // vil2_dicom_file_format_h_
