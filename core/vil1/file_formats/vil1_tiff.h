//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_tiff_file_format_h_
#define vil_tiff_file_format_h_
// Author: awf@robots.ox.ac.uk
// Created: 16 Feb 00
#ifdef __GNUC__
#pragma interface
#endif

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for tiff files
class vil_tiff_file_format : public vil_file_format {
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

//: Generic image implementation for PNM files
class vil_tiff_generic_image : public vil_image_impl {
  vil_stream* is_;
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

  vil_image get_plane(int ) const;
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vil_tiff_file_format.
