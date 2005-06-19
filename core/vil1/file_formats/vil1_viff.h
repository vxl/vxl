// This is core/vil1/file_formats/vil1_viff.h
#ifndef vil1_viff_h_
#define vil1_viff_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Loader for 1-banded Khoros 1.0 images
// \author Peter Vanroose, ESAT, KULeuven.
// \date 17 Feb 2000
// VIFF is the "Visualization Image File Format" used by Khoros 1.0.
//
//\verbatim
//  Modifications
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//  21 February 2002 Maarten Vergauwen - Added access functions for [fi]spare[12]
//  5 June 2003    Peter Vanroose - bug fix for 64-bit platforms: data is 32-bit
//\endverbatim

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>
#include "vil1_viffheader.h"
#include <vxl_config.h> // for vxl_uint_32

//: Loader for VIFF files, i.e., 1-banded Khoros 1.0 images
// This supports char, short, int, float and double pixel cell types.
// No colour support right now.
// Adapted from the version by Reinhard Koch, Nov. 19, 1996.
class vil1_viff_file_format : public vil1_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil1_image_impl* make_input_image(vil1_stream* vs);
  virtual vil1_image_impl* make_output_image(vil1_stream* vs, int planes,
                                             int width,
                                             int height,
                                             int components,
                                             int bits_per_component,
                                             vil1_component_format format);
};

//: Generic image implementation for VIFF (Khoros) files
class vil1_viff_generic_image : public vil1_image_impl
{
  vil1_stream* is_;
  int width_;
  int height_;
  int maxval_;
  int planes_;

  int start_of_data_;
  int bits_per_component_;
  enum vil1_component_format format_;

  bool endian_consistent_;
  vil1_viff_xvimage header_;

  bool read_header();
  bool write_header();
  bool check_endian();

  friend class vil1_viff_file_format;

 public:
  vil1_viff_generic_image(vil1_stream* is);
  vil1_viff_generic_image(vil1_stream* is,
                          int planes,
                          int width,
                          int height,
                          int components,
                          int bits_per_component,
                          vil1_component_format format);
  ~vil1_viff_generic_image();

  //: Dimensions.  Planes x W x H x Components
  virtual int planes() const { return planes_; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return 1; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil1_component_format component_format() const { return format_; }

  //: Copy plane PLANE of this to BUF
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  //: Write BUF into image plane PLANE of this
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);
  //: Emulate a single plane image
  virtual vil1_image get_plane(unsigned int p) const;

  //: Return the image interpreted as rgb bytes.
  virtual bool get_section_rgb_byte(void* buf, int x0, int y0, int width, int height) const;
  virtual bool get_section_float(void* buf, int x0, int y0, int width, int height) const;
  virtual bool get_section_byte(void* buf, int x0, int y0, int width, int height) const;

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;

  //: User defined spare values in header
  vxl_uint_32 ispare1() const { return header_.ispare1;}
  vxl_uint_32 ispare2() const { return header_.ispare2;}
  float fspare1() const { return header_.fspare1;}
  float fspare2() const { return header_.fspare2;}
  void set_ispare1(vxl_uint_32 ispare1);
  void set_ispare2(vxl_uint_32 ispare2);
  void set_fspare1(float fspare1);
  void set_fspare2(float fspare2);
};

#endif // vil1_viff_h_
