#ifndef INSTANTIATE_TEMPLATES
#include <vil1/vil1_rgb.h>
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/vipl_dyadic.txx>

template class vipl_dyadic<vil1_image, vil1_image, vil1_rgb<unsigned char>, vil1_rgb<unsigned char>, vipl_trivial_pixeliter>;
#endif
