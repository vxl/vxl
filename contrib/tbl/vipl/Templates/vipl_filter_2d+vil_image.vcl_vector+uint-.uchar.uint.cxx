#include <vil/vil_image.h>
#include <vcl_vector.h>




// this must be here for filter-2d to work
#include <vipl/section/vipl_filterable_section_container_generator_vil_image.txx>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

#include <vipl/filter/vipl_filter.txx>
template class vipl_filter<vil_image, vcl_vector<unsigned>, unsigned char, unsigned, 2, vipl_trivial_pixeliter>;

#include <vipl/filter/vipl_filter_2d.txx>
template class vipl_filter_2d<vil_image, vcl_vector<unsigned>, unsigned char, unsigned, vipl_trivial_pixeliter>;
