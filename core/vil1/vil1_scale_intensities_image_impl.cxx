// This is core/vil/vil_scale_intensities_image_impl.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_scale_intensities_image_impl.h"
#include <vil/vil_scale_intensities_image.h>
#include <vil/vil_pixel.h>
#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>
#include <vxl_config.h>

vil_image vil_scale_intensities_image_impl::get_plane(int p) const
{
  vil_image_impl *i = new vil_scale_intensities_image_impl(base.get_plane(p), scale_, shift_);
  return i;
}

bool vil_scale_intensities_image_impl::put_section(void const * , int, int, int, int)
{
  return false;
}

bool vil_scale_intensities_image_impl::get_section(void * buf, int x0, int y0, int w, int h) const
{
  switch ( vil_pixel_format(base) ) {
  case VIL_BYTE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_byte*)buf, x0, y0, w, h);
  case VIL_UINT16:
    return vil_scale_intensities_image(base, scale_, shift_, (vxl_uint_16*)buf, x0, y0, w, h);
  case VIL_RGB_BYTE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<vil_byte>*)buf, x0, y0, w, h);
  case VIL_FLOAT:
    return vil_scale_intensities_image(base, scale_, shift_, (float*)buf, x0, y0, w, h);
  case VIL_DOUBLE:
    return vil_scale_intensities_image(base, scale_, shift_, (double*)buf, x0, y0, w, h);
  case VIL_RGB_UINT16:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<vxl_uint_16>*)buf, x0, y0, w, h);
  case VIL_RGB_FLOAT:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<float>*)buf, x0, y0, w, h);
  case VIL_RGB_DOUBLE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<double>*)buf, x0, y0, w, h);
  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type\n";
    return false;
  }
}

/* START_MANCHESTER_BINARY_IO_CODE */

//: Return the name of the class;
vcl_string vil_scale_intensities_image_impl::is_a() const
{
  static const vcl_string class_name_="vil_scale_intensities_image_impl";
  return class_name_;
}

//: Return true if the name of the class matches the argument
bool vil_scale_intensities_image_impl::is_class(vcl_string const& s) const
{
  return s==vil_scale_intensities_image_impl::is_a() || vil_image_impl::is_class(s);
}

/* END_MANCHESTER_BINARY_IO_CODE */
