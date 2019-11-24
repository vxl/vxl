// This is core/vil1/vil1_clamp_image_impl.cxx
//:
// \file

#include "vil1_clamp_image_impl.h"
#include "vil1/vil1_clamp_image.h"
#include "vil1/vil1_pixel.h"
#include "vil1/vil1_rgb.h"
#include "vxl_config.h"

vil1_image vil1_clamp_image_impl::get_plane(unsigned int p) const
{
  vil1_image_impl *i = new vil1_clamp_image_impl(base.get_plane(p), low_, high_);
  return i;
}

bool vil1_clamp_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil1_clamp_image_impl::get_section(void * buf, int x0, int y0, int w, int h) const
{
  switch (vil1_pixel_format(base) ) {
  case VIL1_BYTE:
    return vil1_clamp_image(base, low_, high_, (vxl_byte*)buf, x0, y0, w, h);
  case VIL1_RGB_BYTE:
    return vil1_clamp_image(base, low_, high_, (vil1_rgb<vxl_byte>*)buf, x0, y0, w, h);
  case VIL1_FLOAT:
    return vil1_clamp_image(base, low_, high_, (float*)buf, x0, y0, w, h);
  case VIL1_DOUBLE:
    return vil1_clamp_image(base, low_, high_, (double*)buf, x0, y0, w, h);
  case VIL1_RGB_FLOAT:
    return vil1_clamp_image(base, low_, high_, (vil1_rgb<float>*)buf, x0, y0, w, h);
  case VIL1_RGB_DOUBLE:
    return vil1_clamp_image(base, low_, high_, (vil1_rgb<double>*)buf, x0, y0, w, h);
  default:
    std::cerr << __FILE__ ": not implemented for this pixel type\n";
    return false;
  }
}
