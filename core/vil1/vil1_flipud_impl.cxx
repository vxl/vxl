// This is vxl/vil/vil_flipud_impl.cxx

/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_flipud_impl.h"
#include <vcl_climits.h>

vil_image vil_flipud_impl::get_plane(int i) const {
  return new vil_flipud_impl(base.get_plane(i));
}

bool vil_flipud_impl::get_section(void *buf, int x0, int y0, int w, int h) const {
  int bs = base.components() * base.bits_per_component() / CHAR_BIT;
  for (int j=0; j<h; ++j)
    if (!base.get_section(static_cast<char*>(buf) + bs*w*(h-1-j), x0, y0+j, w, 1))
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
const vcl_string& vil_flipud_impl::is_a() const
{
  const static vcl_string class_name_="vil_flipud_impl";
  return class_name_;
}

/* END_MANCHESTER_BINARY_IO_CODE */

