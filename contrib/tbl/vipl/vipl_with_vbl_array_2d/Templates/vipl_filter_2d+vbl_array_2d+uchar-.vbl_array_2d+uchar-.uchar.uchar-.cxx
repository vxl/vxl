#ifndef INSTANTIATE_TEMPLATES
#include <vbl/vbl_array_2d.h>

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_vbl_array_2d.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vbl_array_2d<unsigned char>, vbl_array_2d<unsigned char>,
                           unsigned char, unsigned char, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vbl_array_2d<unsigned char>, vbl_array_2d<unsigned char>,
                              unsigned char, unsigned char, vipl_trivial_pixeliter>;
#endif
