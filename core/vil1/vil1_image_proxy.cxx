// This is vxl/vil/vil_image_proxy.cxx
#ifdef __GNUC__
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_image_proxy.h"

#include <vcl_string.h>
#include <vcl_cstring.h>
#include <vil/vil_load.h>

struct vil_image_proxy_impl : public vil_image_impl
{
  vcl_string filename;

  // cache this data:
  int planes_;
  int width_;
  int height_;
  int components_;
  int bits_per_component_;
  vil_component_format component_format_;
  vcl_string file_format_;

  vil_image_proxy_impl(char const *file) : filename(file) {
    // cache some stuff
    vil_image I = vil_load(filename.c_str());
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
  enum vil_component_format component_format() const { return component_format_; }
  char const *file_format() const { return file_format_.c_str(); }

  // non-cached stuff
  vil_image get_plane(int i) const { return vil_load(filename.c_str()).get_plane(i); }

  bool get_section(void       *buf, int x0, int y0, int width, int height) const
    { return vil_load(filename.c_str()).get_section(buf, x0, y0, width, height); }
  bool put_section(void const *buf, int x0, int y0, int width, int height)
    { return vil_load(filename.c_str()).put_section(buf, x0, y0, width, height); }

  bool get_property(char const *tag, void       *property_value) const
    { return vil_load(filename.c_str()).get_property(tag, property_value); }
  bool set_property(char const *tag, void const *property_value) const
    { return vil_load(filename.c_str()).set_property(tag, property_value); }

/* START_MANCHESTER_BINARY_IO_CODE */

  //: Return the name of the class;
  virtual vcl_string is_a() const;

  //: Return true if the name of the class matches the argument
  virtual bool is_class(vcl_string const&) const;

/* END_MANCHESTER_BINARY_IO_CODE */
};

vil_image_proxy::vil_image_proxy(char const *file)
  : vil_image(new vil_image_proxy_impl(file))
{
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_image_proxy_impl::is_a() const
{
  static const vcl_string class_name_="vil_image_proxy_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_image_proxy_impl::is_class(vcl_string const& s) const
{
  return s==vil_image_proxy_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
