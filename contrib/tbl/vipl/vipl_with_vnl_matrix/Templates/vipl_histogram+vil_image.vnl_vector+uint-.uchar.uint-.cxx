// for vipl_threshold<...> instantiation:
#include <vnl/vnl_vector.h>
#include <vil/vil_image.h>
#include <vipl/accessors/vipl_accessors_vil_image.txx>
#include "../accessors/vipl_accessors_vnl_vector.h"
#include <vipl/vipl_histogram.txx>
#include "../vipl_filterable_section_container_generator_vnl_vector.txx"

typedef vnl_vector<unsigned int> out_type;

template class vipl_histogram<vil_image,out_type,unsigned char,unsigned, vipl_trivial_pixeliter>;
