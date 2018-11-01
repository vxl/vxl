// This is core/vil1/vil1_flipud_impl.cxx
//:
// \file
// \author fsm

#include "vil1_flipud_impl.h"
#include <climits>

vil1_image vil1_flipud_impl::get_plane(unsigned int p) const {
  return new vil1_flipud_impl(base.get_plane(p));
}

bool vil1_flipud_impl::get_section(void *buf, int x0, int y0, int w, int h) const {
  int bs = base.components() * base.bits_per_component() / CHAR_BIT;
  int last_row = base.height() - 1;
  for (int j=h-1; j>=0; --j)
    if (!base.get_section(static_cast<char*>(buf) + bs*w*j, x0, last_row - y0 - j, w, 1))
      return false;
  return true;
}

bool vil1_flipud_impl::put_section(void const *buf, int x0, int y0, int w, int h) {
  int bs = base.components() * base.bits_per_component() / CHAR_BIT;
  for (int j=0; j<h; ++j)
    if (!base.put_section(static_cast<char const*>(buf) + bs*w*(h-1-j), x0, y0+j, w, 1))
      return false;
  return true;
}

//: Return the name of the class;
std::string vil1_flipud_impl::is_a() const
{
  static const std::string class_name_="vil1_flipud_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil1_flipud_impl::is_class(std::string const& s) const
{
  return s==vil1_flipud_impl::is_a() || vil1_image_impl::is_class(s);
}
