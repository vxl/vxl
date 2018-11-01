// This is core/vil/file_formats/vil_bmp.h
#ifndef vil_bmp_file_format_h_
#define vil_bmp_file_format_h_
//:
// \file
// \author Don Hamilton, Peter Tu
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
// 27 May 2000 fsm Numerous endianness and structure-packing bugs fixed.
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
// 13 November 2011 Gehua Yang - Allow read and write 32bpp image. Default to have the first plane as alpha channel
//                               See a good reference at http://en.wikipedia.org/wiki/BMP_file_format
//\endverbatim

class vil_stream;

//=============================================================================

// Due to padding, you cannot expect to read/write the header
// structures as raw sequences of bytes and still get a valid
// BMP header. The compiler will probably place shorts on 4-byte
// boundaries, which means it will place two bytes of padding
// afterwards (little-endian) or before (bigendian).
//
// Use the read() and write() methods instead.

//--------------------------------------------------------------------------------

#include <vil/vil_file_format.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_stream.h>
#include "vil_bmp_file_header.h"
#include "vil_bmp_core_header.h"
#include "vil_bmp_info_header.h"
class vil_image_view_base;


//: Loader for BMP files
class vil_bmp_file_format : public vil_file_format
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

//: Generic image implementation for BMP files
class vil_bmp_image : public vil_image_resource
{
 public:
  vil_bmp_image(vil_stream* is, unsigned ni,
                unsigned nj, unsigned nplanes, vil_pixel_format format);
  vil_bmp_image(vil_stream* is);
  ~vil_bmp_image() override;

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

  friend class vil_bmp_file_format;

  vil_bmp_file_header file_hdr;
  vil_bmp_core_header core_hdr;
  vil_bmp_info_header info_hdr;
  vil_streampos bit_map_start; // position in file of bitmap raw data.
#if 0
  uchar **freds_colormap;

  xBITMAPINFOHEADER header;
  xBITMAPFILEHEADER fbmp;
  int pixsize;
  int** local_color_map_;
#endif // 0
};


#endif // vil_bmp_file_format_h_
