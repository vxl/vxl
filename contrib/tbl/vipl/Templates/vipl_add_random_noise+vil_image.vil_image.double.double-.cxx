#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_add_random_noise.txx>

template class vipl_add_random_noise<vil_image, vil_image, double, double, vipl_trivial_pixeliter>;
#endif
