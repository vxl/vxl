#ifndef INSTANTIATE_TEMPLATES
#include <iostream>
#include <vector>
#include <section/section.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.hxx"
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.hxx>

#include <vipl/filter/vipl_filter.hxx>
template class vipl_filter<section<unsigned char,2>, std::vector<unsigned int>,
                           unsigned char, unsigned int, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.hxx>
template class vipl_filter_2d<section<unsigned char,2>, std::vector<unsigned int>,
                              unsigned char, unsigned int, vipl_trivial_pixeliter>;
#endif
