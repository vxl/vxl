#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image_view.txx>
#include <vipl/vipl_gaussian_convolution.txx>

template class vipl_gaussian_convolution<vil_image_view<double>, vil_image_view<double>, double, double, vipl_trivial_pixeliter>;
#endif
