#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_scale_intensities_image.h"
#include <vil/vil_image.h>
#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>

template <class T>
bool vil_scale_intensities_image(vil_image const& base, double scale, double shift,
				 T *buf, int x0, int y0, int w, int h)
{
  if (!base.get_section(buf, x0, y0, w, h)) return false;

  unsigned size = w*h;
  for (unsigned pos=0; pos < size; ++pos)
    buf[pos] = T(buf[pos] * scale + shift);

  return true;
}

#undef VIL_INSTANTIATE_SCALE_INTENSITIES_IMAGE
#define VIL_INSTANTIATE_SCALE_INTENSITIES_IMAGE(T) \
template bool vil_scale_intensities_image(vil_image const& base, double scale, double shift, \
			                  T *buf, int x0, int y0, int w, int h)

#undef VIL_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB
#define VIL_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(T) \
VCL_DEFINE_SPECIALIZATION \
bool vil_scale_intensities_image(vil_image const& base, double scale, double shift, \
				 vil_rgb<T> *buf, int x0, int y0, int w, int h) \
{ \
  if (!base.get_section(buf, x0, y0, w, h)) return false; \
  unsigned size = w*h; \
  for (unsigned pos=0; pos < size; ++pos) { \
    buf[pos].R_ = T(buf[pos].R_ * scale + shift); \
    buf[pos].G_ = T(buf[pos].G_ * scale + shift); \
    buf[pos].B_ = T(buf[pos].B_ * scale + shift); \
  } \
  return true; \
}

VIL_INSTANTIATE_SCALE_INTENSITIES_IMAGE(vil_byte);
VIL_INSTANTIATE_SCALE_INTENSITIES_IMAGE(float);
VIL_INSTANTIATE_SCALE_INTENSITIES_IMAGE(double);

VIL_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(vil_byte);
VIL_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(float);
VIL_SPECIALIZE_SCALE_INTENSITIES_IMAGE_RGB(double);
