// This is vxl/vil/file_formats/vil_gen.h
#ifndef vil_gen_file_format_h_
#define vil_gen_file_format_h_
//:
// \file
// \author awf@robots.ox.ac.uk 16 Feb 00
//
// \verbatim
// Modifications
// 10/4/2001 Ian Scott (Manchester) Converted perceps header to doxygen
//  3 October 2001 Peter Vanroose - Implemented get_property("top_row_first")
//  12 Sept. 2002 Peter Vanroose - Changed type of params_ from float to int
//\endverbatim

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

#include <vcl_string.h>
#include <vil/vil_file_format.h>
#include <vil/vil_image_impl.h>

//: Loader for synthetically gend images
// - vil_load("gen:640x480:gray,128");
// - vil_load("gen:640x480:rgb,255,255,255");
class vil_gen_file_format : public vil_file_format
{
 public:
  virtual char const* tag() const;
  virtual vil_image_impl* make_input_image(vil_stream* vs);
  virtual vil_image_impl* make_output_image(vil_stream* /*vs*/, int /*planes*/,
                                            int /*width*/,
                                            int /*height*/,
                                            int /*components*/,
                                            int /*bits_per_component*/,
                                            vil_component_format /*format*/) {
    return 0;
  }
};

enum vil_gen_type {
  vil_gen_gray,
  vil_gen_rgb
};

//: Generic image implementation for PNM files
class vil_gen_generic_image : public vil_image_impl
{
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  int type_;
  int params_[4];

  friend class vil_gen_file_format;
  void init(vcl_string const & s);
 public:

  vil_gen_generic_image(vcl_string const & s);
  vil_gen_generic_image(vcl_string const & s,
                        int planes,
                        int width,
                        int height,
                        int components,
                        int bits_per_component,
                        vil_component_format format);
  ~vil_gen_generic_image();

  //: Dimensions:  planes x width x height x components
  virtual int planes() const { return 1; }
  virtual int width() const { return width_; }
  virtual int height() const { return height_; }
  virtual int components() const { return components_; }

  virtual int bits_per_component() const { return bits_per_component_; }
  virtual enum vil_component_format component_format() const { return VIL_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy contents of this image to buf
  virtual bool get_section(void* buf, int x0, int y0, int width, int height) const;
  //: Copy contents of buf to this image
  virtual bool put_section(void const* buf, int x0, int y0, int width, int height);

  char const* file_format() const;
  bool get_property(char const *tag, void *prop = 0) const;
  vil_image get_plane(int ) const;
};

#endif // vil_gen_file_format_h_
