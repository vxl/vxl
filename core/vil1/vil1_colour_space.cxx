/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_colour_space"
#endif
#include "vil_colour_space.h"
#include <vcl/vcl_cstdlib.h>
#include <vcl/vcl_algorithm.h>

template <class T>
void vil_colour_space_RGB_to_YIQ(T const in[3], T out[3])
{
  out[0] = T(0.299) * in[0] + T(0.587) * in[1] + T(0.114) * in[2];
  out[1] = T(0.596) * in[0] - T(0.275) * in[1] - T(0.321) * in[2];
  out[2] = T(0.212) * in[0] - T(0.523) * in[1] + T(0.311) * in[2];
}

//     green --- yellow     ;
//    /    \     /     \    ;
//  cyan -- white ---- red  ;
//    \    /     \     /    ;
//     blue  --- magenta    ;
//
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

//----------------------------------------------------------------------

#define inst(T) \
template void vil_colour_space_RGB_to_YIQ(T const [3], T [3]); \
template void vil_colour_space_RGB_to_HSV(T, T, T, T *, T *, T *)

inst(double);
