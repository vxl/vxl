#ifndef INSTANTIATE_TEMPLATES
// for section<rgbcell,2> and [gs]etpixel() instantiations:
#include "../accessors/vipl_accessors_section.txx"
#include <vil/vil_rgb.h>
typedef vil_rgb<unsigned char> rgbcell;

VIPL_INSTANTIATE_ACCESSORS(rgbcell);

template class section<rgbcell,2>;
template class section_<rgbcell>;
#endif
