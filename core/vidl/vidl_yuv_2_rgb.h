// This is core/vidl/vidl_yuv_2_rgb.h
#ifndef vidl_yuv_2_rgb_h_
#define vidl_yuv_2_rgb_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm
//
// this file has been copied over from oxl/oxp and optimized a little bit.
// i experimented with  a vul_timer to shave as many milliseconds off as possible.
// l.e.galup  7-15-02
//
// \verbatim
// RGB to YUV Conversion
//
//      Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
//      Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
//      Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
//
// YUV to RGB Conversion
//
//      B = 1.164(Y - 16)                  + 2.018(U - 128)
//      G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
//      R = 1.164(Y - 16) + 1.596(V - 128)

// Alternative:
//      Y = 0.299R + 0.587G + 0.114B
//      U'= (B-Y)*0.565
//      V'= (R-Y)*0.713
//
// with reciprocal versions:
//
//      R = Y + 1.403V'
//      G = Y - 0.344U' - 0.714V'
//      B = Y + 1.770U'
//
// \endverbatim

#include <vil/vil_rgb.h>
#include <vxl_config.h>

const int c1164 = int(1.164 * 1024);
const int c1596 = int(1.596 * 1024);
const int c0813 = int(0.813 * 1024);
const int c0391 = int(0.391 * 1024);
const int c2018 = int(2.018 * 1024);

inline
unsigned char vidl_yuv_2_rgb_byte_clamp(int x)
{
  x = x >> 10;
  if (x < 0) return 0;
  if (x > 255) return 255;
  return (unsigned char) x;
}

inline
void vidl_yuv_2_rgb(unsigned char y, unsigned char u, unsigned char v, unsigned char* rgb)
{
  // int ym16 = c1164*(y-16); replacing this into the below actually takes longer. go figure.
  rgb[0] = vidl_yuv_2_rgb_byte_clamp(c1164*(y - 16) + c1596 * (v - 128));
  rgb[1] = vidl_yuv_2_rgb_byte_clamp(c1164*(y - 16) - c0813 * (v - 128) - c0391 * (u - 128));
  rgb[2] = vidl_yuv_2_rgb_byte_clamp(c1164*(y - 16) + c2018 * (u - 128));
}

inline
void vidl_yuv_2_rgb(unsigned char y, unsigned char u, unsigned char v, vil_rgb<vxl_byte> &vrgb)
{
  vrgb.r = vidl_yuv_2_rgb_byte_clamp(c1164 * (y - 16) + c1596 * (v - 128));
  vrgb.g = vidl_yuv_2_rgb_byte_clamp(c1164 * (y - 16) - c0813 * (v - 128) - c0391 * (u - 128));
  vrgb.b = vidl_yuv_2_rgb_byte_clamp(c1164 * (y - 16) + c2018 * (u - 128));
}

#endif // vidl_yuv_2_rgb_h_
