#include <mil/mil_image_2d_of.h>
typedef unsigned char ubyte;

// this must be here for filter-2d to work
#include "../vipl_filterable_section_container_generator_mil_image.txx"

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<mil_image_2d_of<ubyte>, mil_image_2d_of<ubyte>, ubyte, ubyte, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<mil_image_2d_of<ubyte>, mil_image_2d_of<ubyte>, ubyte, ubyte, vipl_trivial_pixeliter>;
