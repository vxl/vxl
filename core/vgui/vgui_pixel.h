// This is oxl/vgui/vgui_pixel.h
#ifndef vgui_pixel_h_
#define vgui_pixel_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm

#include <vgui/vgui_gl.h>
#include <vxl_config.h>

//: Now we define a bunch of pixel-type structs
// These are *empty* templates. only the specializations make sense.
template <int r, int g, int b> struct vgui_pixel_rgb;
template <int b, int g, int r> struct vgui_pixel_bgr;
template <int r, int g, int b, int a> struct vgui_pixel_rgba;
template <int a, int b, int g, int r> struct vgui_pixel_abgr;
template <int b, int g, int r, int a> struct vgui_pixel_bgra;
struct vgui_pixel_rgbfloat;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgb<8,8,8>
{
  GLubyte R;
  GLubyte G;
  GLubyte B;
};
typedef vgui_pixel_rgb<8,8,8> vgui_pixel_rgb888;

// For 16bit X display.
// With any luck, this will pack into a 16bit word. It works on gcc 2.95,
// but only because we're using 'short' (which is 2 bytes) as the bitfield
// type. If one uses 'char' one gets a 3-byte structure. In one uses 'int'
// the result is a 4-byte structure. So don't change 'short'!
VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgb<5,6,5>
{
#if VXL_LITTLE_ENDIAN
  GLushort B : 5;
  GLushort G : 6;
  GLushort R : 5;
#else
  GLushort R : 5;
  GLushort G : 6;
  GLushort B : 5;
#endif
};
typedef vgui_pixel_rgb<5,6,5> vgui_pixel_rgb565;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_bgr<5,6,5>
{
#if VXL_LITTLE_ENDIAN
  GLushort R : 5;
  GLushort G : 6;
  GLushort B : 5;
#else
  GLushort B : 5;
  GLushort G : 6;
  GLushort R : 5;
#endif
};
typedef vgui_pixel_bgr<5,6,5> vgui_pixel_bgr565;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_bgr<8,8,8>
{
  GLubyte B;
  GLubyte G;
  GLubyte R;
};
typedef vgui_pixel_bgr<8,8,8> vgui_pixel_bgr888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgba<5,5,5,1>
{
  GLushort B:5;
  GLushort G:5;
  GLushort R:5;
  GLushort A:1;
};
typedef vgui_pixel_rgba<5,5,5,1> vgui_pixel_bgra5551;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgba<8,8,8,8>
{
  GLubyte R;
  GLubyte G;
  GLubyte B;
  GLubyte A;
};
typedef vgui_pixel_rgba<8,8,8,8> vgui_pixel_rgba8888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_abgr<8,8,8,8>
{
  GLubyte A;
  GLubyte B;
  GLubyte G;
  GLubyte R;
};
typedef vgui_pixel_abgr<8,8,8,8> vgui_pixel_abgr8888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_bgra<8,8,8,8>
{
  GLubyte B;
  GLubyte G;
  GLubyte R;
  GLubyte A;
};
typedef vgui_pixel_bgra<8,8,8,8> vgui_pixel_bgra8888;


struct vgui_pixel_rgbfloat
{
  float R;
  float G;
  float B;
};


//: Convert a span of pixels from one format to another.
// In general, the input range is assumed to be 0..255, so bitfields
// narrower than 8 bits need to be shifted.  Floats are clamped to 0..255
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(GLubyte const *, vgui_pixel_bgra8888 *, unsigned size);

void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(GLfloat const *, vgui_pixel_bgra8888 *, unsigned size);

void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(GLdouble const *, vgui_pixel_bgra8888 *, unsigned size);

void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgb888 const *, vgui_pixel_bgra8888 *, unsigned size);

void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgba8888 const *, vgui_pixel_bgra8888 *, unsigned size);

void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_rgb888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_bgr888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_rgb565 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_bgra5551 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_rgba8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_abgr8888 *, unsigned size);
void vgui_pixel_convert_span(vgui_pixel_rgbfloat const *, vgui_pixel_bgra8888 *, unsigned size);

#endif // vgui_pixel_h_
