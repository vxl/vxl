#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vil/vil_rgb.h>

typedef vil_rgb<unsigned char> rgb;

VIPL_INSTANTIATE_ACCESSORS( rgb );
#endif
