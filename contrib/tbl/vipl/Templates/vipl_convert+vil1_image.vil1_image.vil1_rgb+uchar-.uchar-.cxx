#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/vipl_convert.txx>
#include <vil1/vil1_rgb.h>

template class vipl_convert<vil1_image, vil1_image, vil1_rgb<unsigned char>, unsigned char, vipl_trivial_pixeliter>;
#endif
