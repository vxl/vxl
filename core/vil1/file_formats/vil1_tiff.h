// This is vxl/vil/file_formats/vil_tiff.h
#ifndef vil_tiff_file_format_h_
#define vil_tiff_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author    awf@robots.ox.ac.uk
// \date 16 Feb 00
//
//\verbatim
//  Modifications
//  3 October 2001 Peter Vanroose - Implemented get_property and set_property
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for tiff files
class vil_tiff_file_format : public vil_file_format
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

struct vil_tiff_structures;

//: Generic image interface for TIFF files
class vil_tiff_generic_image : public vil_image_impl
{
  vil_tiff_structures* p;

  int width_;
  int height_;

  int components_;
  int bits_per_component_;

  bool read_header();
  bool write_header();

  friend class vil_tiff_file_format;
 public:

  vil_tiff_generic_image(vil_stream* is);
  vil_tiff_generic_image(vil_stream* is, int planes,
                                               int width,
                                               int height,
                                               int components,
                                               int bits_per_component,
                                               vil_component_format format);
  ~vil_tiff_generic_image();

  //: TIFF specific fields relating to scanned images
  void get_resolution(float& x_res, float& y_res, unsigned short& units) const;
  void set_resolution(float x_res, float y_res, unsigned short units);

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

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
  bool set_property(char const *tag, const void *prop = 0) const;

  vil_image get_plane(int ) const;
};

#endif // vil_tiff_file_format_h_
