#ifndef INSTANTIATE_TEMPLATES
#include <section/section.h>

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<unsigned char,2>, section<unsigned char,2>,
                           unsigned char, unsigned char, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<unsigned char,2>, section<unsigned char,2>,
                              unsigned char, unsigned char, vipl_trivial_pixeliter>;
#endif
