// This is core/vgui/vgui_pixel.h
#ifndef vgui_pixel_h_
#define vgui_pixel_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief contains struct vgui_pixel_rgb<r,g,b> and other similar ones
// \author fsm
//
// \verbatim
//  Modifications
//   Jan 2003, Amitha Perera: added functionality used in displaying vil images.
//             The vil1 and vil functionality could probably be coalesced for a cleaner design.
// \endverbatim

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
  vgui_pixel_rgb<8,8,8>() { }
  vgui_pixel_rgb<8,8,8>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte /*alpha*/ = 0 )
  : R(red), G(green), B(blue) {}
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
  vgui_pixel_rgb<5,6,5>() { }
  vgui_pixel_rgb<5,6,5>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte /*alpha*/ = 0 )
  : R(red>>3), G(green>>2), B(blue>>3) {}
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
  vgui_pixel_bgr<5,6,5>() { }
  vgui_pixel_bgr<5,6,5>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte /*alpha*/ = 0 )
#if VXL_LITTLE_ENDIAN
  : R(red>>3), G(green>>2), B(blue>>3) {}
#else
  : B(blue>>3), G(green>>2), R(red>>3) {}
#endif

};
typedef vgui_pixel_bgr<5,6,5> vgui_pixel_bgr565;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_bgr<8,8,8>
{
  GLubyte B;
  GLubyte G;
  GLubyte R;
  vgui_pixel_bgr<8,8,8>() {}
  vgui_pixel_bgr<8,8,8>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte /*alpha*/ = 0 )
  : B(blue), G(green), R(red) {}
};
typedef vgui_pixel_bgr<8,8,8> vgui_pixel_bgr888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgba<5,5,5,1>
{
  GLushort B:5;
  GLushort G:5;
  GLushort R:5;
  GLushort A:1;
  vgui_pixel_rgba<5,5,5,1>() { }
  vgui_pixel_rgba<5,5,5,1>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte alpha )
  : B(blue>>3), G(green>>3), R(red>>3), A(alpha) {}
};
typedef vgui_pixel_rgba<5,5,5,1> vgui_pixel_bgra5551;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_rgba<8,8,8,8>
{
  GLubyte R;
  GLubyte G;
  GLubyte B;
  GLubyte A;
  vgui_pixel_rgba<8,8,8,8>() { }
  vgui_pixel_rgba<8,8,8,8>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte alpha )
  : R(red), G(green), B(blue), A(alpha) {}
};
typedef vgui_pixel_rgba<8,8,8,8> vgui_pixel_rgba8888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_abgr<8,8,8,8>
{
  GLubyte A;
  GLubyte B;
  GLubyte G;
  GLubyte R;
  vgui_pixel_abgr<8,8,8,8>() { }
  vgui_pixel_abgr<8,8,8,8>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte alpha )
  : A(alpha), B(blue), G(green), R(red) {}
};
typedef vgui_pixel_abgr<8,8,8,8> vgui_pixel_abgr8888;

VCL_DEFINE_SPECIALIZATION
struct vgui_pixel_bgra<8,8,8,8>
{
  GLubyte B;
  GLubyte G;
  GLubyte R;
  GLubyte A;
  vgui_pixel_bgra<8,8,8,8>() { }
  vgui_pixel_bgra<8,8,8,8>( vxl_byte red, vxl_byte green, vxl_byte blue, vxl_byte alpha )
  : B(blue), G(green), R(red), A(alpha) {}
};
typedef vgui_pixel_bgra<8,8,8,8> vgui_pixel_bgra8888;

struct vgui_pixel_rgbfloat
{
  float R;
  float G;
  float B;
};


//: Clamps the given type into [0,255].
//
template <class T>
inline
vxl_byte vgui_pixel_clamp( T in )
{
  if ( in > 255 ) return 255u;
  if ( in < 0 ) return 0u;
  return vxl_byte(in);
}

// provide overloads for efficiency and to avoid warnings about
// unnecessary comparisons (e.g. against 0 for an unsigned type).

//: Clamps the given type into [0,255].
//
// This overload is the null operation, and is provided for efficiency.
inline
vxl_byte vgui_pixel_clamp( vxl_byte in )
{
  return in;
}

//: Clamps the given type into [0,255].
//
// This overload only checks the lower bound.
inline
vxl_byte vgui_pixel_clamp( vxl_sbyte in )
{
  if ( in < 0 ) return 0u;
  return in;
}

//: Clamps the given type into [0,255].
//
// This overload only checks the upper bound, since the type is
// unsigned.
inline
vxl_byte vgui_pixel_clamp( vxl_uint_16 in )
{
  if ( in > 255 ) return 255u;
  return in;
}

//: Clamps the given type into [0,255].
//
// This overload only checks the upper bound, since the type is
// unsigned.
inline
vxl_byte vgui_pixel_clamp( vxl_uint_32 in )
{
  if ( in > 255 ) return 255u;
  return in;
}


//: Convert the given grey scale value to the appropriate OpenGL pixel type.
template <class InT, class OutT>
inline void
vgui_pixel_convert( InT const& in, OutT& out )
{
  out = OutT( vgui_pixel_clamp( in ), vgui_pixel_clamp( in ),
              vgui_pixel_clamp( in ), 255 );
}

//: Convert the given RGB value to the appropriate OpenGL pixel type.
template <class InT, class OutT>
inline void
vgui_pixel_convert( InT const& R, InT const& G, InT const& B,
                    OutT& out )
{
  out = OutT( vgui_pixel_clamp( R ), vgui_pixel_clamp( G ),
              vgui_pixel_clamp( B ), 255 );
}

//: Convert the given RGBA value to the appropriate OpenGL pixel type.
template <class InT, class OutT>
inline void
vgui_pixel_convert( InT const& R, InT const& G, InT const& B, InT const& A,
                    OutT& out )
{
  out = OutT( vgui_pixel_clamp( R ), vgui_pixel_clamp( G ),
              vgui_pixel_clamp( B ), vgui_pixel_clamp( A ) );
}


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
