// for vipl_histogram<...> instantiations:
#include "../vipl_filterable_section_container_generator_section.txx"
#include "../accessors/vipl_accessors_section.h"
#include <vcl_vector.h>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/vipl_histogram.txx>

template class vipl_histogram<section<unsigned char,2>,vcl_vector<unsigned int>,unsigned char,unsigned int,vipl_trivial_pixeliter>;
