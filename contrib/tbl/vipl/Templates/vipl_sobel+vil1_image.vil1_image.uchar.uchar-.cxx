#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/vipl_sobel.txx>

template class vipl_sobel<vil1_image, vil1_image, unsigned char, unsigned char, vipl_trivial_pixeliter>;
#endif
