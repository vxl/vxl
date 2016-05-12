#ifndef INSTANTIATE_TEMPLATES
// for section<ubyte,2> and [gs]etpixel() instantiations:
#include "../accessors/vipl_accessors_section.hxx"
VIPL_INSTANTIATE_ACCESSORS(unsigned char);

template class section<unsigned char,2>;
template class section_<unsigned char>;

#endif
