#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/vipl_monadic.txx>

template class vipl_monadic<vil1_image, vil1_image, double, double, vipl_trivial_pixeliter>;
#endif
