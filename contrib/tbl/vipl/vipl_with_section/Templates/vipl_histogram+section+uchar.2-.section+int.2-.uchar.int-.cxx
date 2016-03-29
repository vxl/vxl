#ifndef INSTANTIATE_TEMPLATES
// for vipl_histogram<...> instantiations:
#include "../vipl_filterable_section_container_generator_section.hxx"
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_histogram.hxx>

template class vipl_histogram<section<unsigned char,2>,section<int,2>,unsigned char,int, vipl_trivial_pixeliter>;
#endif
