#ifndef INSTANTIATE_TEMPLATES
#include <vil1/vil1_image.h>

#define FILTER_IMPTR_DEC_REFCOUNT(v) {delete (v); (v)=0;}
#define FILTER_IMPTR_INC_REFCOUNT(v)
#define FILTER_KERNPTR_DEC_REFCOUNT(v)

// this must be here for filter-2d to work
#include <vipl/section/vipl_filterable_section_container_generator_vil_image.txx>

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vil1_image, vil1_image, unsigned short, unsigned short, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vil1_image, vil1_image, unsigned short, unsigned short, vipl_trivial_pixeliter>;

#endif
