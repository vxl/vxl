#ifndef INSTANTIATE_TEMPLATES
// for vipl_threshold<...> instantiation:
#include "../vipl_filterable_section_container_generator_section.txx"
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_threshold.txx>
typedef section<unsigned char,2> img_type;

template class vipl_threshold<img_type,img_type,unsigned char,unsigned char, vipl_trivial_pixeliter>;
#endif
