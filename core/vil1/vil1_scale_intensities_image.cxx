// This is core/vil1/vil1_scale_intensities_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_scale_intensities_image.h"
#include <vil1/vil1_image.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h>

template <class T>
bool vil1_scale_intensities_image(vil1_image const& base, double scale, double shift,
                                  T *buf, int x0, int y0, int w, int h)
{
  if (!base.get_section(buf, x0, y0, w, h)) return false;

  unsigned size = w*h;
  for (unsigned pos=0; pos < size; ++pos)
    buf[pos] = T(buf[pos] * scale + shift);

  return true;
}

#undef VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE
#define VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE(T) \
template bool vil1_scale_intensities_image(vil1_image const& base, double scale, double shift, \
                                           T *buf, int x0, int y0, int w, int h)

#undef VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB
#define VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(T) \
VCL_DEFINE_SPECIALIZATION \
bool vil1_scale_intensities_image(vil1_image const& base, double scale, double shift, \
                                  vil1_rgb<T> *buf, int x0, int y0, int w, int h) \
{ \
  if (!base.get_section(buf, x0, y0, w, h)) return false; \
  unsigned size = w*h; \
  for (unsigned pos=0; pos < size; ++pos) { \
    buf[pos].r = (T)(buf[pos].r * scale + shift); \
    buf[pos].g = (T)(buf[pos].g * scale + shift); \
    buf[pos].b = (T)(buf[pos].b * scale + shift); \
  } \
  return true; \
}

VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE(vxl_byte);
VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE(vxl_uint_16);
VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE(float);
VIL1_INSTANTIATE_SCALE_INTENSITIES_IMAGE(double);

VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(vxl_byte);
VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(vxl_uint_16);
VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(float);
VIL1_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(double);
