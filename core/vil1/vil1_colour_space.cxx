// This is core/vil1/vil1_colour_space.cxx
//:
// \file
// \author fsm

#include <cstdlib>
#include <algorithm>
#include <cmath>
#include "vil1_colour_space.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
void vil1_colour_space_RGB_to_YIQ(T const in[3], T out[3])
{
  out[0] = T(0.299) * in[0] + T(0.587) * in[1] + T(0.114) * in[2];
  out[1] = T(0.596) * in[0] - T(0.275) * in[1] - T(0.321) * in[2];
  out[2] = T(0.212) * in[0] - T(0.523) * in[1] + T(0.311) * in[2];
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
void vil1_colour_space_RGB_to_HSV(T r, T g, T b, T *h, T *s, T *v)
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
void vil1_colour_space_HSV_to_RGB(T h, T s, T v, T *r, T *g, T *b)
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
    default: // can never be reached
            break;
  }

  *r = nr * 255; /* Normalize the values to 63 */
  *g = ng * 255;
  *b = nb * 255;
  return;
}


//----------------------------------------------------------------------

#define inst(T) \
template void vil1_colour_space_RGB_to_YIQ(T const [3], T [3]); \
template void vil1_colour_space_RGB_to_HSV(T, T, T, T *, T *, T *); \
template void vil1_colour_space_HSV_to_RGB(T, T, T, T *, T *, T *)

inst(double);
