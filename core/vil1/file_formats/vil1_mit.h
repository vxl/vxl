// This is vxl/vil/file_formats/vil_mit.h
#ifndef vil_mit_file_format_h_
#define vil_mit_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Image in MIT format
// \author  Alison Noble, Oxford University.
//          Joris Schouteden, vil version
//
//   vil_mit is a simple image format consisting of a header
//   of 4 shorts (type,bits_per_pixel,width,height) and the raw data.
//   The full specification defines a number of image/edge types (see header
//   for details). Currently only 8-bit images can be read.
//
// \verbatim
//  Modifications
//     000218 JS  Initial version, header info from MITImage.C
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for MIT files
//
// TYPES:
// -# unsigned (grayscale)
// -# rgb
// -# hsb
// -# cap
// -# signed (grayscale?)
// -# float
// -# edge

class vil_mit_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_impl* make_input_image(vil_stream* vs);
  virtual vil_image_impl* make_output_image(vil_stream* vs, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
};

//: Generic image implementation for MIT files
class vil_mit_generic_image : public vil_image_impl
{
  vil_stream* is_;
  int magic_;
  int width_;
  int height_;
  int maxval_;

  int start_of_data_;
  int components_;
  int bits_per_component_;

  int type_;
  int bits_per_pixel_;

  bool read_header();
  bool write_header();

  friend class vil_mit_file_format;
 public:

  vil_mit_generic_image(vil_stream* is);
  vil_mit_generic_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
  ~vil_mit_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const
  {
    return (type_ != 2) ? bits_per_pixel_ : bits_per_pixel_ / 3;
  }

  virtual int bytes_per_pixel() const
  {
    return bits_per_pixel_ / 8;
  }

  virtual enum vil_component_format component_format() const
  {
    if (type_ == 1 || type_ == 2) return VIL_COMPONENT_FORMAT_UNSIGNED_INT;
    else return VIL_COMPONENT_FORMAT_IEEE_FLOAT;
  }

  //: Copy this to BUF,
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  //: Return the image interpreted as rgb bytes.
  //virtual bool get_section_rgb_byte(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_float(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_byte(void* buf, int x0, int y0, int width, int height) const;

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
  vil_image get_plane(int ) const;
};

#endif // vil_mit_file_format_h_
