// This is core/vil1/vil1_resample.cxx
//:
// \file
// \author fsm

#include "vil1_resample.h"
#include <vil1/vil1_resample_image_impl.h>

vil1_image vil1_resample(vil1_image const &image, int new_width, int new_height)
{
  if (image && new_width>0 && new_height>0)
    return vil1_image(new vil1_resample_image_impl(image, new_width, new_height));
  else
    return vil1_image();
}
