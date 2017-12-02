#ifndef INSTANTIATE_TEMPLATES
// for vipl_threshold<...> instantiation:
#include <vnl/vnl_vector.h>
#include <vil1/vil1_image.h>
#include <vipl/accessors/vipl_accessors_vil1_image.hxx>
#include "../accessors/vipl_accessors_vnl_vector.h"
#include <vipl/vipl_histogram.hxx>
#include "../vipl_filterable_section_container_generator_vnl_vector.hxx"

typedef vnl_vector<unsigned int> out_type;

template class vipl_histogram<vil1_image,out_type,unsigned char,unsigned, vipl_trivial_pixeliter>;
#endif
