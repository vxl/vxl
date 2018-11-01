// This is core/vgui/vgui_pixel.cxx
//:
// \file
// \author fsm
// \brief  See vgui_pixel.h for a description of this file


#include "vgui_pixel.h"
#include <vgui/vgui_gl.h>

//------------------------------------------------------------------------------

template <int M, int N> struct vgui_pixel_assert;
template <> struct vgui_pixel_assert<2, 2> { };
template <> struct vgui_pixel_assert<3, 3> { };
template <> struct vgui_pixel_assert<4, 4> { };
struct vgui_pixel_generates_no_code
{
#define as(T, sz) vgui_pixel_assert<sizeof(T), sz> T##_is_##sz
  as(vgui_pixel_rgb888, 3);
  as(vgui_pixel_bgr888, 3);
  as(vgui_pixel_rgb565, 2);
  as(vgui_pixel_bgra5551, 2);
  as(vgui_pixel_rgba8888, 4);
  as(vgui_pixel_abgr8888, 4);
  as(vgui_pixel_bgra8888, 4);
#undef as
};

//------------------------------------------------------------------------------

// 200201 AWF changed from template magic to an explicit writing out of
//            the 40 conversions.  It's not a template, because the
//            bodies are different in each one.


//: Set an int of unknown size to all-bits-one
#define SET_FF(x) x = 0; x = ~x

//: Clamp float to 0..255
static inline
unsigned int clamp(double f)
{
  if (f > 255) return 255u;
  else if (f < 0) return 0u;
  else return (unsigned int)f;
}

//: This macro is used to generate a "vgui_pixel_convert_span(S*, D*)" function with body "Code"
// Within Code, s is a reference to the source pixel, d to the destination.
// Remember, vgui_pixel_convert_span is not a function template -- each overload has
// rather different code.
#define VGUI_PIXEL_CONVERT_SPAN_DO(S, D, Code) \
  void vgui_pixel_convert_span(S const *src, D *dst, unsigned size) { \
    for (unsigned i=0; i<size; ++i) { \
      S const& s = src[i]; \
      D      & d = dst[i]; \
      Code; \
    }\
  }

// SrcType, DstType, Code
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_rgb888,   { d.R = d.G = d.B = s; } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_bgr888,   { d.R = d.G = d.B = s; } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_rgb565,   { d.R = d.B = (s >> 3); d.G = (s >> 2); } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_bgra5551, { d.R = d.G = d.B = (s>>3); SET_FF(d.A); } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_rgba8888, { d.R = d.G = d.B = s; SET_FF(d.A); } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_abgr8888, { d.R = d.G = d.B = s; SET_FF(d.A); } );
VGUI_PIXEL_CONVERT_SPAN_DO(GLubyte, vgui_pixel_bgra8888, { d.R = d.G = d.B = s; SET_FF(d.A); } );

VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_rgb888,   { d = s; });
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_bgr888,   { d.R = s.R; d.G = s.G; d.B = s.B; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_rgb565,   { d.R = s.R>>3; d.G = s.G>>2; d.B = s.B>>3; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_bgra5551, { d.R = s.R>>3; d.G = s.G>>3; d.B = s.B>>3; SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_rgba8888, { d.R = s.R; d.G = s.G; d.B = s.B; SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_abgr8888, { d.R = s.R; d.G = s.G; d.B = s.B; SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgb888, vgui_pixel_bgra8888, { d.R = s.R; d.G = s.G; d.B = s.B; SET_FF(d.A); })

VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_rgb888,   { d.R = s.R;    d.G = s.G;    d.B = s.B; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_bgr888,   { d.R = s.R;    d.G = s.G;    d.B = s.B; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_rgb565,   { d.R = s.R>>3; d.G = s.G>>2; d.B = s.B>>3; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_bgra5551, { d.R = s.R>>3; d.G = s.G>>3; d.B = s.B>>3; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_rgba8888, { d = s; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_abgr8888, { d.R = s.R;    d.G = s.G;    d.B = s.B; d.A = s.A; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgba8888, vgui_pixel_bgra8888, { d.R = s.R;    d.G = s.G;    d.B = s.B; d.A = s.A; })

VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_rgb888,   { d.R = d.G = d.B = clamp(s); })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_bgr888,   { d.R = d.G = d.B = clamp(s); })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_rgb565,   { int v = clamp(s); d.R = d.B = v>>3; d.G = v>>2; })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_bgra5551, { d.R = d.G = d.B = clamp(s)>>3; SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_rgba8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_abgr8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(float, vgui_pixel_bgra8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })

VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_rgb888,   { d.R = d.G = d.B = clamp(s); })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_bgr888,   { d.R = d.G = d.B = clamp(s); })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_rgb565,   { int v = clamp(s); d.R = d.B = v>>3; d.G = v>>2; })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_bgra5551, { d.R = d.G = d.B = clamp(s)>>3; SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_rgba8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_abgr8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(double, vgui_pixel_bgra8888, { d.R = d.G = d.B = clamp(s); SET_FF(d.A); })

VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_rgb888,  {d.R=clamp(s.R); d.G = clamp(s.G); d.B = clamp(s.B); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_bgr888,  {d.R=clamp(s.R); d.G = clamp(s.G); d.B = clamp(s.B); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_rgb565,  {d.R=clamp(s.R)>>3;d.G =clamp(s.G)>>2; d.B = clamp(s.B)>>3; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_bgra5551,{d.R=clamp(s.R)>>3;d.G =clamp(s.G)>>3; d.B = clamp(s.B)>>3; })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_rgba8888,{d.R=clamp(s.R);d.G=clamp(s.G);d.B=clamp(s.B); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_abgr8888,{d.R=clamp(s.R);d.G=clamp(s.G);d.B=clamp(s.B); SET_FF(d.A); })
VGUI_PIXEL_CONVERT_SPAN_DO(vgui_pixel_rgbfloat, vgui_pixel_bgra8888,{d.R=clamp(s.R);d.G=clamp(s.G);d.B=clamp(s.B); SET_FF(d.A); })
