#ifndef INSTANTIATE_TEMPLATES
// for vipl_dilate_disk<...> instantiation:
#include "../vipl_filterable_section_container_generator_section.hxx"
#include "../accessors/vipl_accessors_section.h"
#include <vipl/vipl_x_gradient.hxx>

#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;
typedef section<rgbcell,2> img_type;

template class vipl_x_gradient<img_type,img_type,rgbcell,rgbcell, vipl_trivial_pixeliter>;
#endif
