/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vgui_pixel.h"
#include <vgui/vgui_gl.h>

//--------------------------------------------------------------------------------

// tunit_init() gets called before anything else in this translation unit.
extern "C" int printf(const char *format, ...);
static bool tunit_init() {
#define as(T,sz) { if (sizeof(T) != sz) printf("%s : %s has size %d, not %d\n", __FILE__, #T, sizeof(T), sz); }
  as(vgui_pixel_rgb888, 3);
  as(vgui_pixel_rgb565, 2);
  as(vgui_pixel_bgr888, 3);
  as(vgui_pixel_rgba8888, 4);
  as(vgui_pixel_abgr8888, 4);
  as(vgui_pixel_bgra8888, 4);
#undef as
  return bool();
}
static bool dummy = tunit_init();

//--------------------------------------------------------------------------------

// macro tragic...  I got confused, and it didn't work on windows, so I did this.  --awf

//template <class S, class D> inline void copy_r(S const* s, D* d) { d->R = s->R; }
//template <class S, class D> inline void copy_g(S const* s, D* d) { d->G = s->G; }
//template <class S, class D> inline void copy_b(S const* s, D* d) { d->B = s->B; }
//template <class S, class D> inline void copy_a(S const* s, D* d) { d->A = s->A; }

// Specialize for uchar...
template <class S> inline void copy_r(S const* s, unsigned char* d) { *d = s->R; }
template <class S> inline void copy_g(S const* s, unsigned char* d) { *d = s->G; }
template <class S> inline void copy_b(S const* s, unsigned char* d) { *d = s->B; }
template <class S> inline void copy_a(S const* s, unsigned char* d) { }

// Source as uchar
template <class D> inline void copy_r(unsigned char const* s, D* d) { d->R = *s; }
template <class D> inline void copy_g(unsigned char const* s, D* d) { d->G = *s; }
template <class D> inline void copy_b(unsigned char const* s, D* d) { d->B = *s; }
template <class D> inline void copy_a(unsigned char const* s, D* d) { d->A = ~0u; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(unsigned char const* s, vgui_pixel_rgb888* d) {  }
VCL_DEFINE_SPECIALIZATION inline void copy_r(unsigned char const* s, vgui_pixel_rgb565* d) { d->R = (*s) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_g(unsigned char const* s, vgui_pixel_rgb565* d) { d->G = (*s) >> 2; }
VCL_DEFINE_SPECIALIZATION inline void copy_b(unsigned char const* s, vgui_pixel_rgb565* d) { d->B = (*s) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(unsigned char const* s, vgui_pixel_rgb565* d) {  }

// Source as float
inline unsigned int clamp(float f) {
  if (f > 255) return 255;
  if (f < 0) return 0;
  return (int)f;
}

template <class D> inline void copy_r(float const* s, D* d) { d->R = clamp(*s); }
template <class D> inline void copy_g(float const* s, D* d) { d->G = clamp(*s); }
template <class D> inline void copy_b(float const* s, D* d) { d->B = clamp(*s); }
template <class D> inline void copy_a(float const* s, D* d) { d->A = ~0u; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(float const* s, vgui_pixel_rgb888* d) {  }
VCL_DEFINE_SPECIALIZATION inline void copy_r(float const* s, vgui_pixel_rgb565* d) { d->R = clamp(*s) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_g(float const* s, vgui_pixel_rgb565* d) { d->G = clamp(*s) >> 2; }
VCL_DEFINE_SPECIALIZATION inline void copy_b(float const* s, vgui_pixel_rgb565* d) { d->B = clamp(*s) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(float const* s, vgui_pixel_rgb565* d) {  }

// Source is 888
template <class D> inline void copy_r(vgui_pixel_rgb888	const* s, D* d) { d->R = s->R; }
template <class D> inline void copy_g(vgui_pixel_rgb888	const* s, D* d) { d->G = s->G; }
template <class D> inline void copy_b(vgui_pixel_rgb888	const* s, D* d) { d->B = s->B; }
template <class D> inline void copy_a(vgui_pixel_rgb888	const* s, D* d) { d->A = ~0u; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(vgui_pixel_rgb888 const* s, vgui_pixel_rgb888* d) {}
VCL_DEFINE_SPECIALIZATION inline void copy_r(vgui_pixel_rgb888 const* s, vgui_pixel_rgb565* d) { d->R = (s->R) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_g(vgui_pixel_rgb888 const* s, vgui_pixel_rgb565* d) { d->G = (s->G) >> 2; }
VCL_DEFINE_SPECIALIZATION inline void copy_b(vgui_pixel_rgb888 const* s, vgui_pixel_rgb565* d) { d->B = (s->B) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(vgui_pixel_rgb888 const* s, vgui_pixel_rgb565* d) { }

template <class D> inline void copy_r(vgui_pixel_rgba8888 const* s, D* d) { d->R = s->R; }
template <class D> inline void copy_g(vgui_pixel_rgba8888 const* s, D* d) { d->G = s->G; }
template <class D> inline void copy_b(vgui_pixel_rgba8888 const* s, D* d) { d->B = s->B; }
template <class D> inline void copy_a(vgui_pixel_rgba8888 const* s, D* d) { d->A = s->A; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(vgui_pixel_rgba8888 const* s, vgui_pixel_rgb888* d) {  }
VCL_DEFINE_SPECIALIZATION inline void copy_r(vgui_pixel_rgba8888 const* s, vgui_pixel_rgb565* d) { d->R = (s->R) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_g(vgui_pixel_rgba8888 const* s, vgui_pixel_rgb565* d) { d->G = (s->G) >> 2; }
VCL_DEFINE_SPECIALIZATION inline void copy_b(vgui_pixel_rgba8888 const* s, vgui_pixel_rgb565* d) { d->B = (s->B) >> 3; }
VCL_DEFINE_SPECIALIZATION inline void copy_a(vgui_pixel_rgba8888 const* s, vgui_pixel_rgb565* d) {  }

//--------------------------------------------------------------------------------

template <class S, class D>
void vgui_pixel_convert_span(S const *src, D *dst, unsigned size) {
  for (unsigned i=0; i<size; ++i) {
    copy_r(src+i, dst+i);
    copy_g(src+i, dst+i);
    copy_b(src+i, dst+i);
    copy_a(src+i, dst+i);
  }
}

#define VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(S, D) \
template void vgui_pixel_convert_span(S const *, D *, unsigned);

VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(GLubyte, vgui_pixel_rgb888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(GLubyte, vgui_pixel_rgb565);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(GLubyte, vgui_pixel_rgba8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(GLubyte, vgui_pixel_abgr8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(GLubyte, vgui_pixel_bgra8888);

VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgb888, vgui_pixel_rgb888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgb888, vgui_pixel_rgb565);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgb888, vgui_pixel_rgba8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgb888, vgui_pixel_abgr8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgb888, vgui_pixel_bgra8888);

VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgba8888, vgui_pixel_rgb888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgba8888, vgui_pixel_rgb565);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgba8888, vgui_pixel_rgba8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgba8888, vgui_pixel_abgr8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(vgui_pixel_rgba8888, vgui_pixel_bgra8888);

VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(float, vgui_pixel_rgb888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(float, vgui_pixel_rgb565);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(float, vgui_pixel_rgba8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(float, vgui_pixel_abgr8888);
VGUI_PIXEL_CONVERT_SPAN_INSTANTIATE(float, vgui_pixel_bgra8888);
