#ifndef INSTANTIATE_TEMPLATES
// for vipl_gaussian_convolution<...> instantiations:
#include "../vipl_filterable_section_container_generator_vnl_matrix.txx"
#include "../accessors/vipl_accessors_vnl_matrix.h"
#include <vipl/vipl_gaussian_convolution.txx>
typedef vnl_matrix<unsigned char> img_type;

template class vipl_gaussian_convolution<img_type,img_type,unsigned char,unsigned char, vipl_trivial_pixeliter>;
#endif
