#ifndef INSTANTIATE_TEMPLATES
#include <vipl/section/vipl_section_descriptor.txx>
#include <vil1/vil1_rgb.h>
typedef vil1_rgb<unsigned char> rgbcell;
template class vipl_section_descriptor<rgbcell>;
#endif
