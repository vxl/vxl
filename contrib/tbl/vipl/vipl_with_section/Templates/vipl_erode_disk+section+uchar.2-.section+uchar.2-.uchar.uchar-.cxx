#ifndef INSTANTIATE_TEMPLATES
// for vipl_erode_disk<...> instantiation:
#include "../vipl_filterable_section_container_generator_section.hxx"
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_erode_disk.hxx>
typedef section<unsigned char,2> img_type;

template class vipl_erode_disk<img_type,img_type,unsigned char,unsigned char, vipl_trivial_pixeliter>;
#endif
