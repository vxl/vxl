#include <vil1/vil1_rgb.h>
#include <vil1/vil1_byte.h>
#include <vil1/vil1_resample_image.txx>

VIL1_RESAMPLE_IMAGE_INSTANTIATE(vil1_rgb<vil1_byte>, vil1_rgb<int>);
