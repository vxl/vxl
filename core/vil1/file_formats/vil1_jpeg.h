//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_jpeg_file_format_h_
#define vil_jpeg_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: fsm@robots.ox.ac.uk
// Created: 17 Feb 2000
// Adapted from geoff's code in ImageClasses/JPEGImage.*

#define fsm_use_stdio_src 0

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_jpeglib.h>

// seeks to 0, then checks for magic number. returns true if found.
bool vil_jpeg_file_probe(vil_stream *vs);

//: Loader for JPEG files
class vil_jpeg_file_format : public vil_file_format {
public:
  virtual char const *tag() const;
  virtual vil_image_impl *make_input_image(vil_stream *vs);
  virtual vil_image_impl *make_output_image(vil_stream *vs,
					       int planes,
					       int width,
					       int height,
					       int components,
					       int bits_per_component,
					       vil_component_format format);
};

//: generic_image implementation for JPEG files
class vil_jpeg_generic_image : public vil_image_impl {
  vil_jpeg_generic_image(vil_stream *is);
  vil_jpeg_generic_image(vil_stream *is,
			 int planes,
			 int width,
			 int height,
			 int components,
			 int bits_per_component,
			 vil_component_format format);
  ~vil_jpeg_generic_image();
  
  //: implementation of virtual interface.
  int planes() const;
  int width() const;
  int height() const;
  int components() const;
  int bits_per_component() const;
  vil_component_format component_format() const;
  char const *file_format() const; // returns "jpeg"
  vil_image get_plane(int ) const;
  bool get_section(void       *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);
  
  //: these methods will access the disk/stream.
  bool decompress_section(void       *buf, int x0, int y0, int w, int h);
  bool   compress_section(void const *buf, int x0, int y0, int w, int h);
  
private:
  bool is_reader;
  vil_stream *stream;
  struct jpeg_error_mgr         jerr;

  struct jpeg_decompress_struct cinfo_d;

  struct jpeg_compress_struct   cinfo_c;
  int proto_bits_per_component;

  friend class vil_jpeg_file_format;
};

#endif
