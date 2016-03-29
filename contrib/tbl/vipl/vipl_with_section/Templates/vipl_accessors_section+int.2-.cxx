#ifndef INSTANTIATE_TEMPLATES
// for section<int,2> and [gs]etpixel() instantiations:
#include "../accessors/vipl_accessors_section.hxx"
VIPL_INSTANTIATE_ACCESSORS(int);

template class section<int,2>;
template class section_<int>;

#endif
