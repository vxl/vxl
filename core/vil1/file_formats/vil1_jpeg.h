// This is core/vil1/file_formats/vil1_jpeg.h
#ifndef vil1_jpeg_file_format_h_
#define vil1_jpeg_file_format_h_
//:
// \file
// \brief Contains classes vil1_jpeg_file_format and vil1_jpeg_generic_image
// \author    fsm
// \date 17 Feb 2000
//
// Adapted from geoff's code in ImageClasses/JPEGImage.*
//
// \verbatim
//  Modifications
//   3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

// seeks to 0, then checks for magic number. returns true if found.
bool vil1_jpeg_file_probe(vil1_stream *vs);

//: Loader for JPEG files
class vil1_jpeg_file_format : public vil1_file_format
{
 public:
  char const *tag() const override;
  vil1_image_impl *make_input_image(vil1_stream *vs) override;
  vil1_image_impl *make_output_image(vil1_stream *vs,
                                             int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format) override;
};

//
class vil1_jpeg_compressor;
class vil1_jpeg_decompressor;

//: generic_image implementation for JPEG files
class vil1_jpeg_generic_image : public vil1_image_impl
{
  vil1_jpeg_generic_image(vil1_stream *is);
  vil1_jpeg_generic_image(vil1_stream *is,
                          int planes,
                          int width,
                          int height,
                          int components,
                          int bits_per_component,
                          vil1_component_format format);
  ~vil1_jpeg_generic_image() override;

  //: implementation of virtual interface.
  int planes() const override;
  int width() const override;
  int height() const override;
  int components() const override;
  int bits_per_component() const override;
  vil1_component_format component_format() const override;
  char const *file_format() const override; // returns "jpeg"
  bool get_property(char const *tag, void *prop = nullptr) const override;
  vil1_image get_plane(unsigned int p) const override;
  bool get_section(void       *buf, int x0, int y0, int w, int h) const override;
  bool put_section(void const *buf, int x0, int y0, int w, int h) override;

 private:
  vil1_jpeg_compressor   *jc;
  vil1_jpeg_decompressor *jd;
  vil1_stream *stream;
  friend class vil1_jpeg_file_format;
};

#endif // vil1_jpeg_file_format_h_
