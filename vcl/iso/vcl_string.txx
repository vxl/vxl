#ifndef vcl_iso_string_txx_
#define vcl_iso_string_txx_
/*
  fsm@robots.ox.ac.uk
*/
#include <vcl/vcl_string.h>

#undef VCL_BASIC_STRING_INSTANTIATE
#define VCL_BASIC_STRING_INSTANTIATE(charT, Traits) \
template class std::basic_string<charT, Traits >;

#endif
