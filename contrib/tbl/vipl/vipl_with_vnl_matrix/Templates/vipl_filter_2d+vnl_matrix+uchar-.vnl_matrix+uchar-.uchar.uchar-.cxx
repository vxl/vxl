#ifndef INSTANTIATE_TEMPLATES
#include <vnl/vnl_matrix.h>

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_vnl_matrix.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vnl_matrix<unsigned char>, vnl_matrix<unsigned char>,
                           unsigned char, unsigned char, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vnl_matrix<unsigned char>, vnl_matrix<unsigned char>,
                              unsigned char, unsigned char, vipl_trivial_pixeliter>;
#endif
