// for vipl_gaussian_convolution<...> instantiations:
#include "../vipl_filterable_section_container_generator_mil_image.txx"
#include "../accessors/vipl_accessors_mil_image.h"
#include <vipl/vipl_gaussian_convolution.txx>
typedef mil_image_2d_of<float> img_type;

template class vipl_gaussian_convolution<img_type,img_type,float,float, vipl_trivial_pixeliter>;
