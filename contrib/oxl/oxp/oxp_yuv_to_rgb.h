// This is oxl/oxp/oxp_yuv_to_rgb.h
#ifndef oxp_yuv_to_rgb_h_
#define oxp_yuv_to_rgb_h_
//:
// \file
// \author fsm
//
// RGB to YUV Conversion
// \code
//      Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
//      Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
//      Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
// \endcode
// YUV to RGB Conversion
// \code
//      B = 1.164(Y - 16)                  + 2.018(U - 128)
//      G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
//      R = 1.164(Y - 16) + 1.596(V - 128)
// \endcode
// Alternative:
// \code
//      Y = 0.299R + 0.587G + 0.114B
//      U'= (B-Y)*0.565
//      V'= (R-Y)*0.713
// \endcode
// with reciprocal versions:
// \code
//      R = Y + 1.403V'
//      G = Y - 0.344U' - 0.714V'
//      B = Y + 1.770U'
// \endcode
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

inline
unsigned char oxp_yuv_to_rgb_byte_clamp(int x)
{
  x = x / 1024;
  if (x < 0) return 0;
  if (x > 255) return 255;
  return (unsigned char) x;
}

inline
void oxp_yuv_to_rgb(unsigned char y, unsigned char u, unsigned char v, unsigned char* rgb)
{
  const int c1164 = int(1.164 * 1024);
  const int c1596 = int(1.596 * 1024);
  const int c0813 = int(0.813 * 1024);
  const int c0391 = int(0.391 * 1024);
  const int c2018 = int(2.018 * 1024);
  rgb[0] = oxp_yuv_to_rgb_byte_clamp(c1164 * (y - 16) + c1596 * (v - 128));
  rgb[1] = oxp_yuv_to_rgb_byte_clamp(c1164 * (y - 16) - c0813 * (v - 128) - c0391 * (u - 128));
  rgb[2] = oxp_yuv_to_rgb_byte_clamp(c1164 * (y - 16) + c2018 * (u - 128));
}

#endif // oxp_yuv_to_rgb_h_
