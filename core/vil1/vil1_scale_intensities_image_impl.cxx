// This is core/vil1/vil1_scale_intensities_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil1_scale_intensities_image_impl.h"
#include <vil1/vil1_scale_intensities_image.h>
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h>

vil1_image vil1_scale_intensities_image_impl::get_plane(unsigned int p) const
{
  vil1_image_impl *i = new vil1_scale_intensities_image_impl(base.get_plane(p), scale_, shift_);
  return i;
}

bool vil1_scale_intensities_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil1_scale_intensities_image_impl::get_section(void * buf, int x0, int y0, int w, int h) const
{
  switch ( vil1_pixel_format(base) ) {
  case VIL1_BYTE:
    return vil1_scale_intensities_image(base, scale_, shift_, (vxl_byte*)buf, x0, y0, w, h);
  case VIL1_UINT16:
    return vil1_scale_intensities_image(base, scale_, shift_, (vxl_uint_16*)buf, x0, y0, w, h);
  case VIL1_RGB_BYTE:
    return vil1_scale_intensities_image(base, scale_, shift_, (vil1_rgb<vxl_byte>*)buf, x0, y0, w, h);
  case VIL1_FLOAT:
    return vil1_scale_intensities_image(base, scale_, shift_, (float*)buf, x0, y0, w, h);
  case VIL1_DOUBLE:
    return vil1_scale_intensities_image(base, scale_, shift_, (double*)buf, x0, y0, w, h);
  case VIL1_RGB_UINT16:
    return vil1_scale_intensities_image(base, scale_, shift_, (vil1_rgb<vxl_uint_16>*)buf, x0, y0, w, h);
  case VIL1_RGB_FLOAT:
    return vil1_scale_intensities_image(base, scale_, shift_, (vil1_rgb<float>*)buf, x0, y0, w, h);
  case VIL1_RGB_DOUBLE:
    return vil1_scale_intensities_image(base, scale_, shift_, (vil1_rgb<double>*)buf, x0, y0, w, h);
  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type\n";
    return false;
  }
}
