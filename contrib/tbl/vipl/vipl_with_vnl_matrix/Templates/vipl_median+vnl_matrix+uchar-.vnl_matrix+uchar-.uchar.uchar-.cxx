#ifndef INSTANTIATE_TEMPLATES
// for vipl_median<...> instantiations:
#include "../vipl_filterable_section_container_generator_vnl_matrix.hxx"
#include "../accessors/vipl_accessors_vnl_matrix.h"
#include <vipl/vipl_median.hxx>
typedef vnl_matrix<unsigned char> img_type;

template class vipl_median<img_type,img_type,unsigned char,unsigned char, vipl_trivial_pixeliter>;
#endif
