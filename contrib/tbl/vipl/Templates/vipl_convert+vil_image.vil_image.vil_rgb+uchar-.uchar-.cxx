#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_convert.txx>
#include <vil/vil_rgb.h>

template class vipl_convert<vil_image, vil_image, vil_rgb<unsigned char>, unsigned char, vipl_trivial_pixeliter>;
#endif
