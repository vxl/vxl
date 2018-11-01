// This is core/vil/file_formats/vil_jpeg.h
#ifndef vil_jpeg_file_format_h_
#define vil_jpeg_file_format_h_
//:
// \file
// \author    fsm
// \date 17 Feb 2000
//
// Adapted from geoff's code in ImageClasses/JPEGImage.*
//
// \verbatim
//  Modifications:
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//     11 Oct 2002 Ian Scott - converted to vil
//\endverbatim

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>

// seeks to 0, then checks for magic number. returns true if found.
bool vil_jpeg_file_probe(vil_stream *vs);

//: Loader for JPEG files
class vil_jpeg_file_format : public vil_file_format
{
 public:
  char const *tag() const override;
  vil_image_resource_sptr make_input_image(vil_stream *vs) override;
  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    enum vil_pixel_format) override;
};

//
class vil_jpeg_compressor;
class vil_jpeg_decompressor;

//: generic_image implementation for JPEG files
class vil_jpeg_image : public vil_image_resource
{
 public:
  vil_jpeg_image(vil_stream *is);
  vil_jpeg_image (vil_stream* is, unsigned ni,
                  unsigned nj, unsigned nplanes, vil_pixel_format format);
  ~vil_jpeg_image() override;

  //: Dimensions:  planes x width x height x components
  unsigned nplanes() const override;
  unsigned ni() const override;
  unsigned nj() const override;

  enum vil_pixel_format pixel_format() const override;

  //: returns "jpeg"
  char const *file_format() const override;

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override;

  bool get_property(char const *tag, void *prop = nullptr) const override;

  //: set the quality for compression
  void set_quality(int quality);

 private:
  vil_jpeg_compressor   *jc;
  vil_jpeg_decompressor *jd;
  vil_stream *stream;
  friend class vil_jpeg_file_format;
};

#endif // vil_jpeg_file_format_h_
