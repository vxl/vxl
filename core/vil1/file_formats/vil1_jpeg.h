//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_jpeg_file_format_h_
#define vil_jpeg_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
// Author: fsm@robots.ox.ac.uk
// Created: 17 Feb 2000
// Adapted from geoff's code in ImageClasses/JPEGImage.*

#include <vil/vil_file_format.h>
#include <vil/vil_generic_image.h>

//: Loader for JPEG files
class vil_jpeg_file_format : public vil_file_format {
public:
  virtual char const *tag() const;
  virtual vil_generic_image *make_input_image(vil_stream *vs);
  virtual vil_generic_image *make_output_image(vil_stream *vs, vil_generic_image const *prototype);
};

//: generic_image implementation for JPEG files
class vil_jpeg_generic_image : public vil_generic_image {
  vil_jpeg_generic_image(vil_stream *is);
  //vil_jpeg_generic_image(vil_stream* is, vil_generic_image const* prototype);
  ~vil_jpeg_generic_image();
  
  //: methods to interrogate the image about its size etc.
  int planes() const;
  int width() const;
  int height() const;
  int components() const;
  int bits_per_component() const;
  vil_component_format component_format() const;
  char const *file_format() const; // returns "jpeg"
  
  //:
  vil_generic_image *get_plane(int ) const;

  //: this method might even spin the disk.
  bool decompress_section(void *buf, int x0, int y0, int width, int height);

  //: this method might cache.
  bool do_get_section(void *buf, int x0, int y0, int width, int height) const;

  //: not currently implemented.
  bool do_put_section(void const *buf, int x0, int y0, int width, int height);

private:
  struct impl;
  impl *pimpl;

  friend class vil_jpeg_file_format;
};

#endif
