// This is ./vxl/vil/file_formats/vil_bmp.h
#ifndef vil_bmp_file_format_h_
#define vil_bmp_file_format_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
// \author Don Hamilton, Peter Tu
// \date 17 Feb 2000
//
//\verbatim
//  Modifications
// 27 May 2000 fsm@robots.ox.ac.uk Numerous endianness and structure-packing bugs fixed.
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vcl_iosfwd.h>
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
#include <vil/vil_image_impl.h>
#include <vil/file_formats/vil_bmp_file_header.h>
#include <vil/file_formats/vil_bmp_core_header.h>
#include <vil/file_formats/vil_bmp_info_header.h>

//: Loader for BMP files
class vil_bmp_file_format : public vil_file_format {
public:
  virtual char const* tag() const;
  virtual vil_image_impl* make_input_image(vil_stream* vs);
  virtual vil_image_impl* make_output_image(vil_stream* vs, int planes,
                                            int width,
                                            int height,
                                            int components,
                                            int bits_per_component,
                                            vil_component_format format);
};

//: Generic image implementation for BMP files
class vil_bmp_generic_image : public vil_image_impl {
public:

  vil_bmp_generic_image(vil_stream* is);
  vil_bmp_generic_image(vil_stream* is,
                        int planes,
                        int width,
                        int height,
                        int components,
                        int bits_per_component,
                        vil_component_format format);

  ~vil_bmp_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return 1; } // assume only one for now.
  virtual int width() const { return core_hdr.width; }
  virtual int height() const { return core_hdr.height; }
  virtual int components() const { return (core_hdr.bitsperpixel<24)?1:core_hdr.bitsperpixel/8; } // FIXME
  virtual int bits_per_component() const { return (core_hdr.bitsperpixel<24)?core_hdr.bitsperpixel:8; } // FIXME
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy plane PLANE of this to BUF,
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

private:
  vil_stream* is_;

  bool read_header();
  bool write_header();

  friend class vil_bmp_file_format;

  vil_bmp_file_header file_hdr;
  vil_bmp_core_header core_hdr;
  vil_bmp_info_header info_hdr;
  unsigned long bit_map_start; // position in file of bitmap raw data.
  //uchar **freds_colormap;

  //xBITMAPINFOHEADER header;
  //xBITMAPFILEHEADER fbmp;
  //int pixsize;
  //int** local_color_map_;
};

#endif // vil_bmp_file_format_h_
