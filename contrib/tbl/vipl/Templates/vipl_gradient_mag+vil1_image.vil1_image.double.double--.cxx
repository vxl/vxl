#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/vipl_gradient_mag.txx>

template class vipl_gradient_mag<vil1_image, vil1_image, double, double, vipl_trivial_pixeliter>;
#endif
