#ifndef INSTANTIATE_TEMPLATES
#include <vnl/vnl_matrix.h>
typedef unsigned char ubyte;

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_vnl_matrix.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vnl_matrix<ubyte>, vnl_matrix<ubyte>, ubyte, ubyte, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vnl_matrix<ubyte>, vnl_matrix<ubyte>, ubyte, ubyte, vipl_trivial_pixeliter>;
#endif
