// This is core/vil1/file_formats/vil1_gen.h
#ifndef vil1_gen_file_format_h_
#define vil1_gen_file_format_h_
//:
// \file
// \author awf@robots.ox.ac.uk 16 Feb 00
//
// \verbatim
//  Modifications
//   10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//   3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//   12 Sept. 2002 Peter Vanroose - Changed type of params_ from float to int
// \endverbatim

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

#include <vcl_string.h>
#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

//: Loader for synthetically gend images
// - vil1_load("gen:640x480:gray,128");
// - vil1_load("gen:640x480:rgb,255,255,255");
class vil1_gen_file_format : public vil1_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil1_image_impl* make_input_image(vil1_stream* vs);
  virtual vil1_image_impl* make_output_image(vil1_stream* /*vs*/, int /*planes*/,
                                             int /*width*/,
                                             int /*height*/,
                                             int /*components*/,
                                             int /*bits_per_component*/,
                                             vil1_component_format /*format*/) { return 0; }
};

enum vil1_gen_type
{
  vil1_gen_gray,
  vil1_gen_rgb
};

//: Generic image implementation for PNM files
class vil1_gen_generic_image : public vil1_image_impl
{
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  int type_;
  int params_[4];

  friend class vil1_gen_file_format;
  void init(vcl_string const & s);

 public:
  vil1_gen_generic_image(vcl_string const & s) { init(s); }
  vil1_gen_generic_image(vcl_string const & s,
                         int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);
  ~vil1_gen_generic_image() {}

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

#endif // vil1_gen_file_format_h_
