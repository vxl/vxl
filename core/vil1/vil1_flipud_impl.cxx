// This is vxl/vil/vil_flipud_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_flipud_impl.h"
#include <vcl_climits.h> // for CHAR_BIT

vil_image vil_flipud_impl::get_plane(int i) const {
  return new vil_flipud_impl(base.get_plane(i));
}

bool vil_flipud_impl::get_section(void *buf, int x0, int y0, int w, int h) const {
  int bs = base.components() * base.bits_per_component() / CHAR_BIT;
  int last_row = base.height() - 1;
  for (int j=h-1; j>=0; --j)
    if (!base.get_section(static_cast<char*>(buf) + bs*w*j, x0, last_row - y0 - j, w, 1))
      return false;
  return true;
}

bool vil_flipud_impl::put_section(void const *buf, int x0, int y0, int w, int h) {
  int bs = base.components() * base.bits_per_component() / CHAR_BIT;
  for (int j=0; j<h; ++j)
    if (!base.put_section(static_cast<char const*>(buf) + bs*w*(h-1-j), x0, y0+j, w, 1))
      return false;
  return true;
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_flipud_impl::is_a() const
{
  static const vcl_string class_name_="vil_flipud_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_flipud_impl::is_class(vcl_string const& s) const
{
  return s==vil_flipud_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
