#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_scale_intensities_image_impl.h"
#include "vil_scale_intensities_image.h"
#include "vil_pixel.h"
#include "vil_rgb.h"
#include "vil_byte.h"

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
  
  switch ( vil_pixel_type(base) ) {
  case VIL_BYTE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_byte*)buf, x0, y0, w, h);
  case VIL_RGB_BYTE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<vil_byte>*)buf, x0, y0, w, h);
  case VIL_FLOAT:
    return vil_scale_intensities_image(base, scale_, shift_, (float*)buf, x0, y0, w, h);
  case VIL_DOUBLE:
    return vil_scale_intensities_image(base, scale_, shift_, (double*)buf, x0, y0, w, h);
  case VIL_RGB_FLOAT:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<float>*)buf, x0, y0, w, h);
  case VIL_RGB_DOUBLE:
    return vil_scale_intensities_image(base, scale_, shift_, (vil_rgb<double>*)buf, x0, y0, w, h);
  default:
    vcl_cerr << __FILE__ ": not implemented for this pixel type" << vcl_endl;
    return false;
  }
}
