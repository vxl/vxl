#include <vil/vil_rgb.h>
#include <vil/vil_byte.h>
#include <vil/vil_resample_image.txx>

VIL_RESAMPLE_IMAGE_INSTANTIATE(vil_rgb<vil_byte>, vil_rgb<int>);
