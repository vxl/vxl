// This is core/vil/file_formats/vil_dicom.h
#ifndef vil_dicom_file_format_h_
#define vil_dicom_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Reader/Writer for DICOM format 2D images.
// \author Amitha Perera
//
// This dicom parser is a wrapper around DCMTK.

#include <vil/vil_image_resource.h>
#include <vil/vil_file_format.h>
#include <vil/file_formats/vil_dicom_header.h>

class DicomImage;
class vil_dicom_stream_input;
class DcmFileFormat;

class vil_image_view_base;


//: Loader for DICOM files
class vil_dicom_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned ni,
                                                    unsigned nj,
                                                    unsigned nplanes,
                                                    vil_pixel_format format);
};


//: Generic image implementation for DICOM files
class vil_dicom_image : public vil_image_resource
{
  vil_dicom_header_info header_;
  vil_image_resource_sptr pixels_;

 public:
  vil_dicom_image(vil_stream* is, unsigned ni,
                   unsigned nj, unsigned nplanes,
                   vil_pixel_format format);
  vil_dicom_image(vil_stream* is);
  ~vil_dicom_image();

  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual enum vil_pixel_format pixel_format() const;

  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  virtual vil_image_view_base_sptr get_view(unsigned i0, unsigned ni,
                                            unsigned j0, unsigned nj) const;

  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

  // Dicom specific stuff
  vil_dicom_header_info const& header() const { return header_; }

  //:correct known manufacturers drop-offs in header data!
  //For example Hologic encode pixel-size in the imageComment!
  //NB if this section starts bloating, use derived classes which override correct_manufacturer_discrepancies
  virtual void correct_manufacturer_discrepancies();
  
  //:try and interpret the hologic comments section to extract pixel size
  virtual bool interpret_hologic_header(float& xpixSize, float& ypixSize);

};

#endif // vil_dicom_file_format_h_
