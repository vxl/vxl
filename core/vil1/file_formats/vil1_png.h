//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_png_file_format_h_
#define vil_png_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: awf@robots.ox.ac.uk
// Created: 17 Feb 00

#include <vil/vil_file_format.h>
#include <vil/vil_generic_image.h>

struct vil_png_structures;

//: Portable network graphics, http://www.cdrom.com/pub/png
class vil_png_file_format : public vil_file_format {
public:
  virtual char const* tag() const;
  virtual vil_generic_image* make_input_image(vil_stream* vs);
  virtual vil_generic_image* make_output_image(vil_stream* vs, vil_generic_image const* prototype);
};

//: Generic image implementation for PNM files
class vil_png_generic_image : public vil_generic_image {
  vil_stream* is_;
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
  vil_png_generic_image(vil_stream* is, vil_generic_image const* prototype);
  ~vil_png_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }
  
  //: Copy this to BUF, 
  virtual bool do_get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool do_put_section(void const* buf, int x0, int y0, int width, int height);
  
  //: Return the image interpreted as rgb bytes.
  //virtual bool get_section_rgb_byte(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_float(void* buf, int x0, int y0, int width, int height) const;
  //virtual bool get_section_byte(void* buf, int x0, int y0, int width, int height) const;

  char const* file_format() const;
  vil_generic_image* get_plane(int ) const;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_png_file_format.
