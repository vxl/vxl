#ifndef INSTANTIATE_TEMPLATES
#include <vcl_vector.h>
#include <vil1/vil1_image.h>
#include <vipl/accessors/vipl_accessors_vil1_image.txx>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/vipl_histogram.txx>
#include <vipl/section/vipl_filterable_section_container_generator_vcl_vector.txx>

typedef vcl_vector<unsigned int> out_type;

template class vipl_histogram<vil1_image,out_type,double,unsigned, vipl_trivial_pixeliter>;
#endif
