#ifndef INSTANTIATE_TEMPLATES
// for section<vil1_rgb<unsigned char>,2> and [gs]etpixel() instantiations:
#include "../accessors/vipl_accessors_section.txx"
#include <vil1/vil1_rgb.h>

VIPL_INSTANTIATE_ACCESSORS(vil1_rgb<unsigned char>);

template class section<vil1_rgb<unsigned char>,2>;
template class section_<vil1_rgb<unsigned char>>;
#endif
