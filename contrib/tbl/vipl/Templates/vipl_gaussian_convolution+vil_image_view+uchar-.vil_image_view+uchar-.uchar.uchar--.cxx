#ifndef INSTANTIATE_TEMPLATES
#include <vipl/accessors/vipl_accessors_vil_image_view.txx>
#include <vipl/vipl_gaussian_convolution.txx>

template class vipl_gaussian_convolution<vil_image_view<unsigned char>, vil_image_view<unsigned char>,
                                         unsigned char, unsigned char, vipl_trivial_pixeliter>;
#endif
