#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_gradient_mag.txx>

template class vipl_gradient_mag<vil_image, vil_image, float, float, vipl_trivial_pixeliter>;
#endif
