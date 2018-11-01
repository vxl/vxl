// This is core/vil1/file_formats/vil1_bmp.h
#ifndef vil1_bmp_file_format_h_
#define vil1_bmp_file_format_h_
//:
// \file
// \author Don Hamilton, Peter Tu
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
// 27 May 2000 fsm Numerous endianness and structure-packing bugs fixed.
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

class vil1_stream;

//=============================================================================

// Due to padding, you cannot expect to read/write the header
// structures as raw sequences of bytes and still get a valid
// BMP header. The compiler will probably place shorts on 4-byte
// boundaries, which means it will place two bytes of padding
// afterwards (little-endian) or before (bigendian).
//
// Use the read() and write() methods instead.

//--------------------------------------------------------------------------------

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>
#include "vil1_bmp_file_header.h"
#include "vil1_bmp_core_header.h"
#include "vil1_bmp_info_header.h"

//: Loader for BMP files
class vil1_bmp_file_format : public vil1_file_format
{
 public:
  char const* tag() const override;
  vil1_image_impl* make_input_image(vil1_stream* vs) override;
  vil1_image_impl* make_output_image(vil1_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format) override;
};

//: Generic image implementation for BMP files
class vil1_bmp_generic_image : public vil1_image_impl
{
 public:

  vil1_bmp_generic_image(vil1_stream* is);
  vil1_bmp_generic_image(vil1_stream* is,
                         int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);

  ~vil1_bmp_generic_image() override;

  //: Dimensions.  Planes x W x H x Components
  int planes() const override { return 1; } // assume only one for now.
  int width() const override { return core_hdr.width; }
  int height() const override { return core_hdr.height; }
  int components() const override { return (core_hdr.bitsperpixel<24)?1:core_hdr.bitsperpixel/8; } // FIXME
  int bits_per_component() const override { return (core_hdr.bitsperpixel<24)?core_hdr.bitsperpixel:8; } // FIXME
  enum vil1_component_format component_format() const override { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy plane PLANE of this to BUF,
  bool get_section(void* buf, int x0, int y0, int width, int height) const override;
  bool put_section(void const* buf, int x0, int y0, int width, int height) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;

 private:
  vil1_stream* is_;

  bool read_header();
  bool write_header();

  friend class vil1_bmp_file_format;

  vil1_bmp_file_header file_hdr;
  vil1_bmp_core_header core_hdr;
  vil1_bmp_info_header info_hdr;
  long bit_map_start; // position in file of bitmap raw data.
#if 0
  uchar **freds_colormap;

  xBITMAPINFOHEADER header;
  xBITMAPFILEHEADER fbmp;
  int pixsize;
  int** local_color_map_;
#endif
};

#endif // vil1_bmp_file_format_h_
