// This is core/vil/file_formats/vil_sgi.h
#ifndef vil_sgi_file_format_h_
#define vil_sgi_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
  virtual char const* tag() const;
  virtual vil_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil_image_resource_sptr make_output_image(vil_stream* vs,
                                                    unsigned nx,
                                                    unsigned ny,
                                                    unsigned nplanes,
                                                    vil_pixel_format format);
};

//: Generic image implementation for SGI files
class vil_sgi_image : public vil_image_resource
{
 public:
  vil_sgi_image(vil_stream* is, unsigned ni,
                unsigned nj, unsigned nplanes, vil_pixel_format format);
  vil_sgi_image(vil_stream* is);
  ~vil_sgi_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const;
  virtual unsigned ni() const;
  virtual unsigned nj() const;

  virtual enum vil_pixel_format pixel_format() const {return VIL_PIXEL_FORMAT_BYTE; }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                 unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = nullptr) const;
 private:
  vil_stream* is_;

  bool read_header();
  bool write_header();

  friend class vil_sgi_file_format;

  vil_sgi_file_header hdr;
};


#endif // vil_sgi_file_format_h_
