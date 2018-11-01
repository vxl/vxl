// This is core/vil1/file_formats/vil1_mit.h
#ifndef vil1_mit_file_format_h_
#define vil1_mit_file_format_h_
//:
// \file
// \brief Image in MIT format
// \author  Alison Noble, Oxford University.
//          Joris Schouteden, vil1 version
//
//   vil1_mit is a simple image format consisting of a header
//   of 4 shorts (type,bits_per_pixel,width,height) and the raw data.
//   The full specification defines a number of image/edge types (see header
//   for details). Currently only 8-bit images can be read.
//
// \verbatim
//  Modifications
//     000218 JS  Initial version, header info from MITImage.C
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

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

class vil1_mit_file_format : public vil1_file_format
{
 public:
  char const* tag() const override;
  vil1_image_impl* make_input_image(vil1_stream* vs) override;
  vil1_image_impl* make_output_image(vil1_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format) override;
};

//: Generic image implementation for MIT files
class vil1_mit_generic_image : public vil1_image_impl
{
  vil1_stream* is_;
  int width_;
  int height_;

  int components_;
  int bits_per_component_;

  int type_;
  int bits_per_pixel_;

  bool read_header();
  bool write_header();

  friend class vil1_mit_file_format;
 public:

  vil1_mit_generic_image(vil1_stream* is);
  vil1_mit_generic_image(vil1_stream* is, int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);
  ~vil1_mit_generic_image() override;

  //: Dimensions.  Planes x W x H x Components
  int planes() const override { return 1; }
  int width() const override { return width_; }
  int height() const override { return height_; }
  int components() const override { return components_; }

  int bits_per_component() const override
  {
    return (type_ != 2) ? bits_per_pixel_ : bits_per_pixel_ / 3;
  }

  virtual int bytes_per_pixel() const
  {
    return bits_per_pixel_ / 8;
  }

  enum vil1_component_format component_format() const override
  {
    if (type_ == 1 || type_ == 2) return VIL1_COMPONENT_FORMAT_UNSIGNED_INT;
    else return VIL1_COMPONENT_FORMAT_IEEE_FLOAT;
  }

  //: Copy this to BUF,
  bool get_section(void* buf, int x0, int y0, int width, int height) const override;
  bool put_section(void const* buf, int x0, int y0, int width, int height) override;

  //: Return the image interpreted as rgb bytes.
  //virtual bool get_section_rgb_byte(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_float(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_byte(void* buf, int x0, int y0, int width, int height) const;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;
  vil1_image get_plane(unsigned int p) const override;
};

#endif // vil1_mit_file_format_h_
