// This is vxl/vil/file_formats/vil_png.h
#ifndef vil_png_file_format_h_
#define vil_png_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author   awf@robots.ox.ac.uk
// \date 17 Feb 00
//
//\verbatim
//  Modifications
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

struct vil_png_structures;

//: Portable network graphics, http://www.cdrom.com/pub/png
class vil_png_file_format : public vil_file_format
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

//: Generic image implementation for PNG files
class vil_png_generic_image : public vil_image_impl
{
  vil_stream* vs_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;

  int start_of_data_;

  bool read_header();
  bool write_header();

  friend class vil_png_file_format;
  vil_png_structures* p;
 public:

  vil_png_generic_image(vil_stream* is);
  vil_png_generic_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
  ~vil_png_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }

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

#endif // vil_png_file_format_h_
