// This is vxl/vil/vil_pixel.cxx

/*
  awf@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vil_pixel.h"
#include <vil/vil_image.h>

vil_pixel_format_t vil_pixel_format(vil_image const &I)
{
  int c = I.components();
  int b = I.bits_per_component();
  vil_component_format f = I.component_format();

  // assume 8 = CHAR_BIT
  if (c == 1 && b ==  8 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_BYTE;
  if (c == 1 && b == 16 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_UINT16;
  if (c == 1 && b == 24 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_RGB_BYTE; // fsm: bmp?
  if (c == 1 && b == 32 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_UINT32;
  if (c == 1 && b == 32 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL_FLOAT;
  if (c == 1 && b == 64 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL_DOUBLE;
  if (c == 1 && b == 64 && f == VIL_COMPONENT_FORMAT_COMPLEX)      return VIL_COMPLEX;
  if (c == 3 && b ==  8 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_RGB_BYTE;
  if (c == 3 && b == 16 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_RGB_UINT16;
  if (c == 3 && b == 32 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL_RGB_FLOAT;
  if (c == 3 && b == 64 && f == VIL_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL_RGB_DOUBLE;
  if (c == 4 && b ==  8 && f == VIL_COMPONENT_FORMAT_UNSIGNED_INT) return VIL_RGBA_BYTE;

  return VIL_PIXEL_FORMAT_UNKNOWN;
}
