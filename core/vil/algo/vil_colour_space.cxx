// This is core/vil/algo/vil_colour_space.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vil_colour_space.h"
#include <vcl_cstdlib.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>

template <class T>
void vil_colour_space_RGB_to_YIQ(T const in[3], T out[3])
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
void vil_colour_space_RGB_to_HSV(T r, T g, T b, T *h, T *s, T *v)
{
  T max = vcl_max(r, vcl_max(g, b));
  T min = vcl_min(r, vcl_min(g, b));

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
      vcl_abort();

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

  if (h == 360)
  h = 0;           /* (THIS LOOKS BACKWARDS)       */

  xh = h / 60;                   // convert hue to be in [0,6)
  i = (int)vcl_floor((double)xh);// i = greatest integer <= xh
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
  }

  *r = nr * 255; /* Normalize the values to 63 */
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

//----------------------------------------------------------------------

#define inst(T) \
template void vil_colour_space_RGB_to_YIQ(T const [3], T [3]); \
template void vil_colour_space_RGB_to_HSV(T, T, T, T *, T *, T *); \
template void vil_colour_space_HSV_to_RGB(T, T, T, T *, T *, T *); \
template void vil_colour_space_RGB_to_YUV(T const [3], T [3]); \
template void vil_colour_space_YUV_to_RGB(T const [3], T [3])

inst(double);
inst(float);
