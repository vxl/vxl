#ifndef INSTANTIATE_TEMPLATES
#include <vipl/section/vipl_section_iterator.txx>
#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;
template class vipl_section_iterator<rgbcell>;
#endif
