#ifndef INSTANTIATE_TEMPLATES
#include <section/section.h>
#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_section.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<section<rgbcell,2>, section<rgbcell,2>, rgbcell, rgbcell, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<section<rgbcell,2>, section<rgbcell,2>, rgbcell, rgbcell, vipl_trivial_pixeliter>;
#endif
