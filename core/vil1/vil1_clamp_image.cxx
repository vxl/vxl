// This is vxl/vil/vil_clamp_image.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil_clamp_image.h"
#include <vil/vil_image.h>
#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>

#undef VIL_CLAMP_IMAGE_THRESHOLD
#define VIL_CLAMP_IMAGE_THRESHOLD(V, L, H) \
  if (V > H) V = H; \
  else if (V < L) V = L

template <class T>
bool vil_clamp_image(vil_image const& base, double low, double high,
                     T *buf, int x0, int y0, int w, int h)
{
  if (!base.get_section(buf, x0, y0, w, h)) return false;

  unsigned size = w*h;
  T low_thresh = T(low);
  T high_thresh = T(high);
  for (unsigned pos=0; pos < size; ++pos) {
    VIL_CLAMP_IMAGE_THRESHOLD(buf[pos], low_thresh, high_thresh);
  }
  return true;
}

#undef VIL_INSTANTIATE_CLAMP_IMAGE
#define VIL_INSTANTIATE_CLAMP_IMAGE(T) \
template bool vil_clamp_image(vil_image const& base, double low, double high, \
                              T *buf, int x0, int y0, int w, int h)

#undef VIL_SPECIALIZE_CLAMP_IMAGE_RGB
#define VIL_SPECIALIZE_CLAMP_IMAGE_RGB(T) \
VCL_DEFINE_SPECIALIZATION \
bool vil_clamp_image(vil_image const& base, double low, double high, \
                       vil_rgb<T> *buf, int x0, int y0, int w, int h) \
{ \
  if (!base.get_section(buf, x0, y0, w, h)) return false; \
  unsigned size = w*h; \
  T low_thresh = T(low); \
  T high_thresh = T(high); \
  for (unsigned pos=0; pos < size; ++pos) { \
    VIL_CLAMP_IMAGE_THRESHOLD(buf[pos].r, low_thresh, high_thresh); \
    VIL_CLAMP_IMAGE_THRESHOLD(buf[pos].g, low_thresh, high_thresh); \
    VIL_CLAMP_IMAGE_THRESHOLD(buf[pos].b, low_thresh, high_thresh); \
  } \
  return true; \
}

VIL_INSTANTIATE_CLAMP_IMAGE(vil_byte);
VIL_INSTANTIATE_CLAMP_IMAGE(float);
VIL_INSTANTIATE_CLAMP_IMAGE(double);

VIL_SPECIALIZE_CLAMP_IMAGE_RGB(vil_byte);
VIL_SPECIALIZE_CLAMP_IMAGE_RGB(float);
VIL_SPECIALIZE_CLAMP_IMAGE_RGB(double);
