//-*- c++ -*-------------------------------------------------------------------
#ifndef vil_jpeg_file_format_h_
#define vil_jpeg_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif

//:
// \file
// \author    fsm@robots.ox.ac.uk
// \date 17 Feb 2000
//
// Adapted from geoff's code in ImageClasses/JPEGImage.*
//
// \verbatim
//  Modifications:
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

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

//
class vil_jpeg_compressor;
class vil_jpeg_decompressor;

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
  bool get_property(char const *tag, void *prop = 0) const;
  vil_image get_plane(int ) const;
  bool get_section(void       *buf, int x0, int y0, int w, int h) const;
  bool put_section(void const *buf, int x0, int y0, int w, int h);

private:
  vil_jpeg_compressor   *jc;
  vil_jpeg_decompressor *jd;
  vil_stream *stream;
  friend class vil_jpeg_file_format;
};

#endif // vil_jpeg_file_format_h_
