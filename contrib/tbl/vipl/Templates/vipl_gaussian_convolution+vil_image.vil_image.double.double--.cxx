#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/vipl_gaussian_convolution.txx>

template class vipl_gaussian_convolution<vil_image, vil_image, double, double, vipl_trivial_pixeliter>;
#endif
