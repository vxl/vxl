// This is core/vil/algo/vil_colour_space.cxx
//:
// \file
// \author fsm

#include <cstdlib>
#include <algorithm>
#include <cmath>
#include "vil_colour_space.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void vil_colour_space_RGB_to_YIQ(T const in[3], T out[3])
{
  out[0] = T(0.299) * in[0] + T(0.587) * in[1] + T(0.114) * in[2];
  out[1] = T(0.595716) * in[0] - T(0.274453) * in[1] - T(0.321263) * in[2];
  out[2] = T(0.211456) * in[0] - T(0.522591) * in[1] + T(0.311135) * in[2];
}

template <class T>
void vil_colour_space_YIQ_to_RGB(T const in[3], T out[3])
{
  out[0] = in[0] + T(0.956296) * in[1] + T(0.621024) * in[2];
  out[1] = in[0] - T(0.272122) * in[1] - T(0.647381) * in[2];
  out[2] = in[0] - T(1.106989) * in[1] + T(1.704615) * in[2];
}

//:
// \verbatim
//     green --- yellow     //
//    /    \     /     \    //
// cyan --- white ---- red  //
//    \    /     \     /    //
//     blue  --- magenta    //
//                          //
// \endverbatim

template <class T>
void vil_colour_space_RGB_to_HSV(T r, T g, T b, T *h, T *s, T *v)
{
  T max = std::max(r, std::max(g, b));
  T min = std::min(r, std::min(g, b));

  // The value v is just the maximum.
  *v = max;

  // Next, saturation.
  if (max > 0)
    *s = (max - min)/max;
  else
    *s = 0;

  // Lastly, the hue:
  if (*s == 0)
    *h = T(); // The hue is undefined in the achromatic case.
  else {
    T delta = max - min;
    if      (r == max)
      *h = (g - b)/delta;
    else if (g == max)
      *h = 2 + (b - r)/delta;
    else if (b == max)
      *h = 4 + (r - g)/delta;
    else
      std::abort();

    *h *= 60;
    if (*h < 0)
      *h += 360;
  }
}

template <class T>
void vil_colour_space_HSV_to_RGB(T h, T s, T v, T *r, T *g, T *b)
{
  T p1, p2, p3, f, nr=0, ng=0, nb=0;
  T xh;
  int i;

  v = v/255;

#if 0
  extern float hue,  s,  v;  // hue (0.0 to 360.0, is circular, 0=360)
                             // s and v are from 0.0 to 1.0
  extern long  r2,  g2,  b2; // values from 0 to 63
#endif

  h -= int(h/360)*360;       // (THIS LOOKS BACKWARDS)
  if (h < 0) h += 360;

  xh = h / 60;                   // convert hue to be in [0,6)
  i = (int)std::floor((double)xh);// i = greatest integer <= xh
  f = xh - i;                    // f = fractional part of xh
  p1 = v * (1 - s);
  p2 = v * (1 - (s * f));
  p3 = v * (1 - (s * (1 - f)));

  switch (i)
  {
    case 0:
            nr = v;
            ng = p3;
            nb = p1;
            break;
    case 1:
            nr = p2;
            ng = v;
            nb = p1;
            break;
    case 2:
            nr = p1;
            ng = v;
            nb = p3;
            break;
    case 3:
            nr = p1;
            ng = p2;
            nb = v;
            break;
    case 4:
            nr = p3;
            ng = p1;
            nb = v;
            break;
    case 5:
            nr = v;
            ng = p1;
            nb = p2;
            break;
    default:
            break; // cannot be reached
  }

  *r = nr * 255; // Normalize the values to 63
  *g = ng * 255;
  *b = nb * 255;
  return;
}


template <class T>
void vil_colour_space_RGB_to_YUV(T const in[3], T out[3])
{
  out[0] = T(0.299) * in[0] + T(0.587) * in[1] + T(0.114) * in[2];
  out[1] = T(0.492) * (in[2] - out[0]);
  out[2] = T(0.877) * (in[0] - out[0]);
}


template <class T>
void vil_colour_space_YUV_to_RGB(T const in[3], T out[3])
{
  // the coefficient of the inverse are given here to higher precision
  // than typically used.  This allows for more accurate results when
  // working with floating point color representations
  out[0] = in[0] + T(1.1402508551881) * in[2];
  out[1] = in[0] - T(0.39473137491174) * in[1] - T(0.5808092090311) * in[2];
  out[2] = in[0] + T(2.0325203252033) * in[1];
}

// YPbPr (ITU-R BT.601)
// ========================================================
// Y' =     + 0.299    * R' + 0.587    * G' + 0.114    * B'
// Pb =     - 0.168736 * R' - 0.331264 * G' + 0.5      * B'
// Pr =     + 0.5      * R' - 0.418688 * G' - 0.081312 * B'
// ........................................................
// R', G', B' in [0; 1]
// Y' in [0; 1]
// Pb in [-0.5; 0.5]
// Pr in [-0.5; 0.5]
template <class T>
void vil_colour_space_RGB_to_YPbPr_601(T const RGB[3], T YPbPr[3])
{
  YPbPr[0] = T(0.299)    * RGB[0] + T(0.587)    * RGB[1] + T(0.114)    * RGB[2];
  YPbPr[1] = T(-0.168736)* RGB[0] - T(0.331264) * RGB[1] + T(0.5)      * RGB[2];
  YPbPr[2] = T(0.5)      * RGB[0] - T(0.418688) * RGB[1] - T(0.081312) * RGB[2];
}

// YPbPr (ITU-R BT.601)
// ==========================================================
// R' =     + 0.299    * Y' + 0.587    * Pb' + 0.114    * Pr'
// G' =     - 0.168736 * Y' - 0.331264 * Pb' + 0.5      * Pr'
// B' =     + 0.5      * Y' - 0.418688 * Pb' - 0.081312 * Pr'
// ..........................................................
// Y' in [0; 1]
// Pb in [-0.5; 0.5]
// Pr in [-0.5; 0.5]
// R', G', B' in [0; 1]
template <class T>
void vil_colour_space_YPbPr_601_to_RGB(T const YPbPr[3], T RGB[3])
{
  RGB[0] = std::max(T(0.0), std::min(T(1.0), (YPbPr[0]                          + T(1.402)    * YPbPr[2])));
  RGB[1] = std::max(T(0.0), std::min(T(1.0), (YPbPr[0] - T(0.344136) * YPbPr[1] - T(0.714136) * YPbPr[2])));
  RGB[2] = std::max(T(0.0), std::min(T(1.0), (YPbPr[0] + T(1.772)    * YPbPr[1]                         )));
}

// YCbCr (601) from "digital 8-bit R'G'B'  "
// ========================================================================
// Y' = 16  + 1/256 * (   65.738  * R'd +  129.057  * G'd +  25.064  * B'd)
// Cb = 128 + 1/256 * ( - 37.945  * R'd -   74.494  * G'd + 112.439  * B'd)
// Cr = 128 + 1/256 * (  112.439  * R'd -   94.154  * G'd -  18.285  * B'd)
// ........................................................................
// R'd, G'd, B'd in {0, 1, 2, ..., 255}
// Y'               in {16, 17, ..., 235}
//    with footroom in {1, 2, ..., 15}
//         headroom in {236, 237, ..., 254}
//         sync.    in {0, 255}
// Cb, Cr           in {16, 17, ..., 240}
void vil_colour_space_RGB_to_YCbCr_601(const unsigned char RGB[3], unsigned char YCbCr[3])
{
  // Add an extra 0.5555555 to round instead of truncate
  YCbCr[0] = static_cast<unsigned char>(
    16.0  + (  65.738 * RGB[0] + 129.057 * RGB[1] +  25.064 * RGB[2])/256.0 + 0.55555555);
  YCbCr[1] = static_cast<unsigned char>(
    128.0 + ( -37.945 * RGB[0] -  74.494 * RGB[1] + 112.439 * RGB[2])/256.0 + 0.55555555);
  YCbCr[2] = static_cast<unsigned char>(
    128.0 + ( 112.439 * RGB[0] -  94.154 * RGB[1] -  18.285 * RGB[2])/256.0 + 0.55555555);
}

// 8-bit R'G'B' from YCbCr (601)
// =====================================================================
// R'd = ( 298.082 * Y'                + 408.583 * Cr ) / 256 - 222.921
// G'd = ( 298.082 * Y' - 100.291 * Cb - 208.120 * Cr ) / 256 + 135.576
// B'd = ( 298.082 * Y' + 516.412 * Cb                ) / 256 - 276.836
void vil_colour_space_YCbCr_601_to_RGB(const unsigned char YCbCr[3], unsigned char RGB[3])
{
  RGB[0] = static_cast<unsigned char>(
    (298.082 * YCbCr[0]                      + 408.583 * YCbCr[2]) / 256.0 - 222.921);
  RGB[1] = static_cast<unsigned char>(
    (298.082 * YCbCr[0] - 100.291 * YCbCr[1] - 208.120 * YCbCr[2]) / 256.0 + 135.576);
  RGB[2] = static_cast<unsigned char>(
    (298.082 * YCbCr[0] + 516.412 * YCbCr[1]                     ) / 256.0 - 276.836);
}


//----------------------------------------------------------------------

#define inst(T) \
template void vil_colour_space_RGB_to_YIQ(T const [3], T [3]); \
template void vil_colour_space_YIQ_to_RGB(T const [3], T [3]); \
template void vil_colour_space_RGB_to_HSV(T, T, T, T *, T *, T *); \
template void vil_colour_space_HSV_to_RGB(T, T, T, T *, T *, T *); \
template void vil_colour_space_RGB_to_YUV(T const [3], T [3]); \
template void vil_colour_space_YUV_to_RGB(T const [3], T [3]); \
template void vil_colour_space_RGB_to_YPbPr_601(T const RGB[3], T YPbPr[3]); \
template void vil_colour_space_YPbPr_601_to_RGB(T const YPbPr[3], T RGB[3])

inst(double);
inst(float);
