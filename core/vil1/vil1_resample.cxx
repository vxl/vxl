// This is vxl/vil/vil_resample.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vil_resample.h"
#include <vil/vil_resample_image_impl.h>

vil_image vil_resample(vil_image const &image, int new_width, int new_height)
{
  if (image && new_width>0 && new_height>0)
    return vil_image(new vil_resample_image_impl(image, new_width, new_height));
  else
    return vil_image();
}
