#ifndef INSTANTIATE_TEMPLATES
// for vipl_gaussian_convolution<...> instantiations:
#include "../vipl_filterable_section_container_generator_vnl_matrix.txx"
#include "../accessors/vipl_accessors_vnl_matrix.h"
#include <vipl/vipl_gaussian_convolution.txx>
typedef unsigned char ubyte;
typedef vnl_matrix<ubyte> img_type;

template class vipl_gaussian_convolution<img_type,img_type,ubyte,ubyte, vipl_trivial_pixeliter>;
#endif
