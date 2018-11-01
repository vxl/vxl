// This is core/vil1/vil1_pixel.cxx
//:
// \file
// \author awf@robots.ox.ac.uk

#include "vil1_pixel.h"
#include <vil1/vil1_image.h>

vil1_pixel_format_t vil1_pixel_format(vil1_image const &I)
{
  int c = I.components();
  int b = I.bits_per_component();
  vil1_component_format f = I.component_format();

  // assume 8 = CHAR_BIT
  if (c == 1 && b ==  8 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_BYTE;
  if (c == 1 && b == 16 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_UINT16;
  if (c == 1 && b == 24 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_RGB_BYTE; // fsm: bmp?
  if (c == 1 && b == 32 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_UINT32;
  if (c == 1 && b == 32 && f == VIL1_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL1_FLOAT;
  if (c == 1 && b == 64 && f == VIL1_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL1_DOUBLE;
  if (c == 1 && b == 64 && f == VIL1_COMPONENT_FORMAT_COMPLEX)      return VIL1_COMPLEX;
  if (c == 3 && b ==  8 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_RGB_BYTE;
  if (c == 3 && b == 16 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_RGB_UINT16;
  if (c == 3 && b == 32 && f == VIL1_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL1_RGB_FLOAT;
  if (c == 3 && b == 64 && f == VIL1_COMPONENT_FORMAT_IEEE_FLOAT)   return VIL1_RGB_DOUBLE;
  if (c == 4 && b ==  8 && f == VIL1_COMPONENT_FORMAT_UNSIGNED_INT) return VIL1_RGBA_BYTE;

  return VIL1_PIXEL_FORMAT_UNKNOWN;
}
