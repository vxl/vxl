#ifndef vcl_egcs_string_txx_
#define vcl_egcs_string_txx_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_string.h>

#undef VCL_BASIC_STRING_INSTANTIATE
#define VCL_BASIC_STRING_INSTANTIATE(charT, Traits) \
template ostream& operator<<(ostream&, basic_string<charT, Traits > const &);

#endif
