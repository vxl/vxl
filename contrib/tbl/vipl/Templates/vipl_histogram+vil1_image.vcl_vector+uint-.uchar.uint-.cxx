#ifndef INSTANTIATE_TEMPLATES
// for vipl_threshold<...> instantiation:
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/vipl_histogram.txx>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

template class vipl_histogram<vil1_image,vcl_vector<unsigned int>,unsigned char,unsigned, vipl_trivial_pixeliter>;
#endif
