#ifndef INSTANTIATE_TEMPLATES
#include <section/section.h>
#include <vcl_vector.h>

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.txx"
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<unsigned char,2>, vcl_vector<unsigned int>,
                           unsigned char, unsigned int, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<unsigned char,2>, vcl_vector<unsigned int>,
                              unsigned char, unsigned int, vipl_trivial_pixeliter>;
#endif
