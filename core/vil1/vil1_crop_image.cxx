#ifdef __GNUC__
#pragma implementation "vil_crop_image.h"
#endif

#include "vil_crop_image.h"

vil_crop_image::vil_crop_image(vil_generic_image* gi, int x0, int y0, int w, int h):
  gi_(gi),
  x0_(x0),
  y0_(y0),
  width_(w),
  height_(h)
{
}

vil_crop_image::~vil_crop_image()
{
}
