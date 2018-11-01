// This is core/vil/file_formats/vil_sgi.h
#ifndef vil_sgi_file_format_h_
#define vil_sgi_file_format_h_
//:
// \file
// \author David Hughes
// \date 16 May 2018


#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>
#include "vil_sgi_file_header.h"

//=============================================================================

//: Loader for SGI files
class vil_sgi_file_format : public vil_file_format
{
 public:
  char const* tag() const override;
  vil_image_resource_sptr make_input_image(vil_stream* vs) override;
  vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    vil_pixel_format format) override;
};

//: Generic image implementation for SGI files
class vil_sgi_image : public vil_image_resource
{
 public:
  vil_sgi_image(vil_stream* is, unsigned ni,
                unsigned nj, unsigned nplanes, vil_pixel_format format);
  vil_sgi_image(vil_stream* is);
  ~vil_sgi_image() override;

  //: Dimensions:  planes x width x height x components
  unsigned nplanes() const override;
  unsigned ni() const override;
  unsigned nj() const override;

  enum vil_pixel_format pixel_format() const override {return VIL_PIXEL_FORMAT_BYTE; }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const override;

  //: Put the data in this view back into the image source.
  bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;
 private:
  vil_stream* is_;

  bool read_header();
  bool write_header();

  friend class vil_sgi_file_format;

  vil_sgi_file_header hdr;
};


#endif // vil_sgi_file_format_h_
