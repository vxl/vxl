// This is core/vil1/vil1_image_proxy.cxx
//:
// \file
// \author fsm

#include <string>
#include "vil1_image_proxy.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil1/vil1_load.h>

struct vil1_image_proxy_impl : public vil1_image_impl
{
  std::string filename;

  // cache this data:
  int planes_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil1_component_format component_format_;
  std::string file_format_;

  vil1_image_proxy_impl(char const *file) : filename(file) {
    // cache some stuff
    vil1_image I = vil1_load(filename.c_str());
#define macro(p) p##_ = I.p()
    macro(planes);
    macro(width);
    macro(height);
    macro(components);
    macro(bits_per_component);
    macro(component_format);
#undef macro
    file_format_ = I.file_format();
  }

  // cached stuff
  int planes() const override { return planes_; }
  int width() const override { return width_; }
  int height() const override { return height_; }
  int components() const override { return components_; }
  int bits_per_component() const override { return bits_per_component_; }
  enum vil1_component_format component_format() const override { return component_format_; }
  char const *file_format() const override { return file_format_.c_str(); }

  // non-cached stuff
  vil1_image get_plane(unsigned int p) const override { return vil1_load(filename.c_str()).get_plane(p); }

  bool get_section(void       *buf, int x0, int y0, int width, int height) const override
  { return vil1_load(filename.c_str()).get_section(buf, x0, y0, width, height); }
  bool put_section(void const *buf, int x0, int y0, int width, int height) override
  { return vil1_load(filename.c_str()).put_section(buf, x0, y0, width, height); }

  bool get_property(char const *tag, void       *property_value) const override
  { return vil1_load(filename.c_str()).get_property(tag, property_value); }
  bool set_property(char const *tag, void const *property_value) const override
  { return vil1_load(filename.c_str()).set_property(tag, property_value); }

  //: Return the name of the class;
  std::string is_a() const override;

  //: Return true if the name of the class matches the argument
  bool is_class(std::string const&) const override;
};

vil1_image_proxy::vil1_image_proxy(char const *file)
  : vil1_image(new vil1_image_proxy_impl(file))
{
}

//: Return the name of the class;
std::string vil1_image_proxy_impl::is_a() const
{
  static const std::string class_name_="vil1_image_proxy_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_image_proxy_impl::is_class(std::string const& s) const
{
  return s==vil1_image_proxy_impl::is_a() || vil1_image_impl::is_class(s);
}
