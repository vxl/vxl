// This is core/vil1/vil1_image_proxy.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil1_image_proxy.h"
#include <vcl_string.h>
#include <vil1/vil1_load.h>

struct vil1_image_proxy_impl : public vil1_image_impl
{
  vcl_string filename;

  // cache this data:
  int planes_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil1_component_format component_format_;
  vcl_string file_format_;

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
  int planes() const { return planes_; }
  int width() const { return width_; }
  int height() const { return height_; }
  int components() const { return components_; }
  int bits_per_component() const { return bits_per_component_; }
  enum vil1_component_format component_format() const { return component_format_; }
  char const *file_format() const { return file_format_.c_str(); }

  // non-cached stuff
  vil1_image get_plane(unsigned int p) const { return vil1_load(filename.c_str()).get_plane(p); }

  bool get_section(void       *buf, int x0, int y0, int width, int height) const
  { return vil1_load(filename.c_str()).get_section(buf, x0, y0, width, height); }
  bool put_section(void const *buf, int x0, int y0, int width, int height)
  { return vil1_load(filename.c_str()).put_section(buf, x0, y0, width, height); }

  bool get_property(char const *tag, void       *property_value) const
  { return vil1_load(filename.c_str()).get_property(tag, property_value); }
  bool set_property(char const *tag, void const *property_value) const
  { return vil1_load(filename.c_str()).set_property(tag, property_value); }

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;
};

vil1_image_proxy::vil1_image_proxy(char const *file)
  : vil1_image(new vil1_image_proxy_impl(file))
{
}

//: Return the name of the class;
vcl_string vil1_image_proxy_impl::is_a() const
{
  static const vcl_string class_name_="vil1_image_proxy_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_image_proxy_impl::is_class(vcl_string const& s) const
{
  return s==vil1_image_proxy_impl::is_a() || vil1_image_impl::is_class(s);
}
