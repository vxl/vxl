#include <section/section.h>
#include <vcl_vector.h>
typedef unsigned char ubyte;

// this must be here for filter-2d to work
#include "vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx"
// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<ubyte,2>, vcl_vector<unsigned>, ubyte, unsigned, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<ubyte,2>, vcl_vector<unsigned>, ubyte, unsigned, vipl_trivial_pixeliter>;

