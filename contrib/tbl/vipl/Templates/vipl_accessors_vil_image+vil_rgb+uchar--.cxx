#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vil1/vil1_rgb.h>

typedef vil1_rgb<unsigned char> rgb;

VIPL_INSTANTIATE_ACCESSORS( rgb );
#endif
