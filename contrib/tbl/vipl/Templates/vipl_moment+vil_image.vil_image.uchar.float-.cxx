#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_moment.txx>

template class vipl_moment<vil_image, vil_image, unsigned char, float, vipl_trivial_pixeliter>;
#endif
