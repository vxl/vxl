// This is vxl/vil/vil_resample.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_resample.h"
#include <vil/vil_resample_image_impl.h>

vil_image vil_resample(vil_image const &image, unsigned new_width, unsigned new_height)
{
  if (image && new_width && new_height)
    return vil_image(new vil_resample_image_impl(image, new_width, new_height));
  else
    return vil_image();
}
