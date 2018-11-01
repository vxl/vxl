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


#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_file_format.h>
#include <vil1/vil1_image_impl.h>

//: Loader for synthetically gend images
// - vil1_load("gen:640x480:gray,128");
// - vil1_load("gen:640x480:rgb,255,255,255");
class vil1_gen_file_format : public vil1_file_format
{
 public:
  char const* tag() const override;
  vil1_image_impl* make_input_image(vil1_stream* vs) override;
  vil1_image_impl* make_output_image(vil1_stream* /*vs*/, int /*planes*/,
                                             int /*width*/,
                                             int /*height*/,
                                             int /*components*/,
                                             int /*bits_per_component*/,
                                             vil1_component_format /*format*/) override { return nullptr; }
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
  void init(std::string const & s);

 public:
  vil1_gen_generic_image(std::string const & s) { init(s); }
  vil1_gen_generic_image(std::string const & s,
                         int planes,
                         int width,
                         int height,
                         int components,
                         int bits_per_component,
                         vil1_component_format format);
  ~vil1_gen_generic_image() override = default;

  //: Dimensions:  planes x width x height x components
  int planes() const override { return 1; }
  int width() const override { return width_; }
  int height() const override { return height_; }
  int components() const override { return components_; }

  int bits_per_component() const override { return bits_per_component_; }
  enum vil1_component_format component_format() const override { return VIL1_COMPONENT_FORMAT_UNSIGNED_INT; }

  //: Copy contents of this image to buf
  bool get_section(void* buf, int x0, int y0, int width, int height) const override;
  //: Copy contents of buf to this image
  bool put_section(void const* buf, int x0, int y0, int width, int height) override;

  char const* file_format() const override;
  bool get_property(char const *tag, void *prop = nullptr) const override;
  vil1_image get_plane(unsigned int p) const override;
};

#endif // vil1_gen_file_format_h_
