// This is mul/vil2/file_formats/vil2_bmp.h
#ifndef vil2_bmp_file_format_h_
#define vil2_bmp_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
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

#include <vil2/vil2_file_format.h>
#include <vil2/vil2_image_resource.h>
#include "vil2_bmp_file_header.h"
#include "vil2_bmp_core_header.h"
#include "vil2_bmp_info_header.h"
class vil2_image_view_base;


//: Loader for BMP files
class vil2_bmp_file_format : public vil2_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil2_image_resource_sptr make_input_image(vil_stream* vs);
  virtual vil2_image_resource_sptr make_output_image(vil_stream* vs,
                                                 unsigned nx,
                                                 unsigned ny,
                                                 unsigned nplanes,
                                                 vil2_pixel_format format);
};

//: Generic image implementation for BMP files
class vil2_bmp_image : public vil2_image_resource
{
 public:
  vil2_bmp_image (vil_stream* is, unsigned ni,
    unsigned nj, unsigned nplanes, vil2_pixel_format format);
  vil2_bmp_image(vil_stream* is);
  ~vil2_bmp_image();

  //: Dimensions:  planes x width x height x components
  virtual unsigned nplanes() const {
    return (core_hdr.bitsperpixel<24)?1:core_hdr.bitsperpixel/8; }  // FIXME
  virtual unsigned ni() const { return core_hdr.width; }
  virtual unsigned nj() const { return core_hdr.height; }

  virtual enum vil2_pixel_format pixel_format() const {return VIL2_PIXEL_FORMAT_BYTE; }

  //: Create a read/write view of a copy of this data.
  // \return 0 if unable to get view of correct size.
  virtual vil2_image_view_base_sptr get_copy_view(unsigned i0, unsigned ni,
                                                  unsigned j0, unsigned nj) const;

  //: Put the data in this view back into the image source.
  virtual bool put_view(const vil2_image_view_base& im, unsigned i0, unsigned j0);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
 private:
  vil_stream* is_;

  bool read_header();
  bool write_header();

  friend class vil2_bmp_file_format;

  vil2_bmp_file_header file_hdr;
  vil2_bmp_core_header core_hdr;
  vil2_bmp_info_header info_hdr;
  long bit_map_start; // position in file of bitmap raw data.
#if 0
  uchar **freds_colormap;

  xBITMAPINFOHEADER header;
  xBITMAPFILEHEADER fbmp;
  int pixsize;
  int** local_color_map_;
#endif
};


#endif // vil2_bmp_file_format_h_
