#ifndef INSTANTIATE_TEMPLATES
// for vipl_gradient_mag<...> instantiations:
#include "../vipl_filterable_section_container_generator_vbl_array_2d.hxx"
#include "../accessors/vipl_accessors_vbl_array_2d.h"
#include <vipl/vipl_gradient_mag.hxx>
typedef vbl_array_2d<unsigned char> img_type;

template class vipl_gradient_mag<img_type,img_type,unsigned char,unsigned char, vipl_trivial_pixeliter>;
#endif
