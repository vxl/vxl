// This is ./vxl/vil/vil_clamp_image_impl.cxx
#ifdef __GNUC__
#pragma implementation
#endif

//:
// \file

#include "vil_clamp_image_impl.h"
#include "vil_clamp_image.h"
#include "vil_pixel.h"
#include "vil_rgb.h"
#include "vil_byte.h"

vil_image vil_clamp_image_impl::get_plane(int p) const
{
  vil_image_impl *i = new vil_clamp_image_impl(base.get_plane(p), low_, high_);
  return i;
}

bool vil_clamp_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil_clamp_image_impl::get_section(void * buf, int x0, int y0, int w, int h) const
{
  switch (vil_pixel_format(base) ) {
  case VIL_BYTE:
    return vil_clamp_image(base, low_, high_, (vil_byte*)buf, x0, y0, w, h);
  case VIL_RGB_BYTE:
    return vil_clamp_image(base, low_, high_, (vil_rgb<vil_byte>*)buf, x0, y0, w, h);
  case VIL_FLOAT:
    return vil_clamp_image(base, low_, high_, (float*)buf, x0, y0, w, h);
  case VIL_DOUBLE:
    return vil_clamp_image(base, low_, high_, (double*)buf, x0, y0, w, h);
  case VIL_RGB_FLOAT:
    return vil_clamp_image(base, low_, high_, (vil_rgb<float>*)buf, x0, y0, w, h);
  case VIL_RGB_DOUBLE:
    return vil_clamp_image(base, low_, high_, (vil_rgb<double>*)buf, x0, y0, w, h);
  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type" << vcl_endl;
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_clamp_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_clamp_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_clamp_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_clamp_image_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
