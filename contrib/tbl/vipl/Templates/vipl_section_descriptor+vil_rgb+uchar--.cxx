#ifndef INSTANTIATE_TEMPLATES
#include <vipl/section/vipl_section_descriptor.txx>
#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;
template class vipl_section_descriptor<rgbcell>;
#endif
