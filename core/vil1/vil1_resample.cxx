/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_resample"
#endif
#include "vil_resample.h"
#include <vil/vil_resample_image_impl.h>

vil_image vil_resample(vil_image const &image, unsigned new_width, unsigned new_height)
{
  if (image && new_width && new_height)
    return vil_image(new vil_resample_image_impl(image, new_width, new_height));
  else
    return vil_image();
}
