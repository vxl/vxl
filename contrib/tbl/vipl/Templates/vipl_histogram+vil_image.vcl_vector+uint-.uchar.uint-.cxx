#ifndef INSTANTIATE_TEMPLATES
// for vipl_threshold<...> instantiation:
#include <vcl_vector.h>
#include <vil/vil_image.h>
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/vipl_histogram.txx>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

typedef vcl_vector<unsigned int> out_type;

template class vipl_histogram<vil_image,out_type,unsigned char,unsigned, vipl_trivial_pixeliter>;
#endif
