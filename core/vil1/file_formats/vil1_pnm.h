// This is core/vil1/file_formats/vil1_pnm.h
#ifndef vil1_pnm_file_format_h_
#define vil1_pnm_file_format_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author awf@robots.ox.ac.uk
// \date 16 Feb 00
//
// \verbatim
// Modifications
// 7 June 2001 - Peter Vanroose - made pbm (magic P4) working
// 3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//\endverbatim

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

//: Loader for PPM,PGM,PBM files
class vil1_pnm_file_format : public vil1_file_format
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

//: Generic image implementation for PNM files
class vil1_pnm_generic_image : public vil1_image_impl
{
  vil1_stream* vs_;
  int magic_;
  int width_;
  int height_;
  unsigned long int maxval_;

  int start_of_data_;
  int components_;
  int bits_per_component_;

  bool read_header();
  bool write_header();

  friend class vil1_pnm_file_format;
 public:

  vil1_pnm_generic_image(vil1_stream* is);
  vil1_pnm_generic_image(vil1_stream* is, int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);
  ~vil1_pnm_generic_image();

  //: Dimensions:  planes x width x height x components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil1_component_format component_format() const { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy contents of this image to buf
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  //: Copy contents of buf to this image
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
  vil1_image get_plane(unsigned int p) const;
};

#endif // vil1_pnm_file_format_h_
