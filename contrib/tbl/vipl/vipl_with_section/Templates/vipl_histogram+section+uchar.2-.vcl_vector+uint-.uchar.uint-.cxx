// for vipl_histogram<...> instantiations:
#include "../vipl_filterable_section_container_generator_section.txx"
#include "vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx"
#include "../accessors/vipl_accessors_section.h"
#include "vipl/accessors/vipl_accessors_vcl_vector.h"
#include <vipl/vipl_histogram.txx>

template class vipl_histogram<section<unsigned char,2>,vcl_vector<unsigned>,unsigned char,unsigned, vipl_trivial_pixeliter>;
