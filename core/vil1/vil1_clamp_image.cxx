// This is core/vil1/vil1_clamp_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_clamp_image.h"
#include <vil1/vil1_image.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h>

#undef VIL1_CLAMP_IMAGE_THRESHOLD
#define VIL1_CLAMP_IMAGE_THRESHOLD(V, L, H) \
  if (V > H) V = H; \
  else if (V < L) V = L

template <class T>
bool vil1_clamp_image(vil1_image const& base, double low, double high,
                      T *buf, int x0, int y0, int w, int h)
{
  if (!base.get_section(buf, x0, y0, w, h)) return false;

  unsigned size = w*h;
  T low_thresh = T(low);
  T high_thresh = T(high);
  for (unsigned pos=0; pos < size; ++pos) {
    VIL1_CLAMP_IMAGE_THRESHOLD(buf[pos], low_thresh, high_thresh);
  }
  return true;
}

#undef VIL1_INSTANTIATE_CLAMP_IMAGE
#define VIL1_INSTANTIATE_CLAMP_IMAGE(T) \
template bool vil1_clamp_image(vil1_image const& base, double low, double high, \
                               T *buf, int x0, int y0, int w, int h)

#undef VIL1_SPECIALIZE_CLAMP_IMAGE_RGB
#define VIL1_SPECIALIZE_CLAMP_IMAGE_RGB(T) \
VCL_DEFINE_SPECIALIZATION \
bool vil1_clamp_image(vil1_image const& base, double low, double high, \
                      vil1_rgb<T> *buf, int x0, int y0, int w, int h) \
{ \
  if (!base.get_section(buf, x0, y0, w, h)) return false; \
  unsigned size = w*h; \
  T low_thresh = T(low); \
  T high_thresh = T(high); \
  for (unsigned pos=0; pos < size; ++pos) { \
    VIL1_CLAMP_IMAGE_THRESHOLD(buf[pos].r, low_thresh, high_thresh); \
    VIL1_CLAMP_IMAGE_THRESHOLD(buf[pos].g, low_thresh, high_thresh); \
    VIL1_CLAMP_IMAGE_THRESHOLD(buf[pos].b, low_thresh, high_thresh); \
  } \
  return true; \
}

VIL1_INSTANTIATE_CLAMP_IMAGE(vxl_byte);
VIL1_INSTANTIATE_CLAMP_IMAGE(float);
VIL1_INSTANTIATE_CLAMP_IMAGE(double);

VIL1_SPECIALIZE_CLAMP_IMAGE_RGB(vxl_byte);
VIL1_SPECIALIZE_CLAMP_IMAGE_RGB(float);
VIL1_SPECIALIZE_CLAMP_IMAGE_RGB(double);
