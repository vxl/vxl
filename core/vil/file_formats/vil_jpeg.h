// This is mul/vil2/file_formats/vil2_jpeg.h
#ifndef vil2_jpeg_file_format_h_
#define vil2_jpeg_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
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
//     11 Oct 2002 Ian Scott - converted to vil22
//\endverbatim

#include <vil2/vil2_file_format.h>
#include <vil2/vil2_image_resource.h>

// seeks to 0, then checks for magic number. returns true if found.
bool vil2_jpeg_file_probe(vil_stream *vs);

//: Loader for JPEG files
class vil2_jpeg_file_format : public vil2_file_format
{
 public:
  virtual char const *tag() const;
  virtual vil2_image_resource_sptr make_input_image(vil_stream *vs);
  virtual vil2_image_resource_sptr make_output_image(vil_stream* vs,
                                                     unsigned nx,
                                                     unsigned ny,
                                                     unsigned nplanes,
                                                     enum vil2_pixel_format);
};

//
class vil2_jpeg_compressor;
class vil2_jpeg_decompressor;

//: generic_image implementation for JPEG files
class vil2_jpeg_image : public vil2_image_resource
{
  vil2_jpeg_image(vil_stream *is);
  vil2_jpeg_image (vil_stream* is, unsigned ni,
    unsigned nj, unsigned nplanes, vil2_pixel_format format);
  ~vil2_jpeg_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;
 
  virtual enum vil2_pixel_format pixel_format() const;

  //: returns "jpeg"
  char const *file_format() const;

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  bool get_property(char const *tag, void *prop = 0) const;

 private:
  vil2_jpeg_compressor   *jc;
  vil2_jpeg_decompressor *jd;
  vil_stream *stream;
  friend class vil2_jpeg_file_format;
};

#endif // vil2_jpeg_file_format_h_
