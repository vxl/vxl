#ifdef __GNUC__
#pragma implementation "vil_crop_image_impl.h"
#endif

#include "vil_crop_image_impl.h"

vil_crop_image_impl::vil_crop_image_impl(vil_image const& gi, int x0, int y0, int w, int h):
  gi_(gi),
  x0_(x0),
  y0_(y0),
  width_(w),
  height_(h)
{
}

vil_crop_image_impl::~vil_crop_image_impl()
{
}
