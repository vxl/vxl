#include <vcl/vcl_functional.h>
#include <vcl/vcl_string.h>
#include <vcl/vcl_set.txx>

#ifndef __alpha
VCL_SET_INSTANTIATE(vcl_string, vcl_less<vcl_string> );
#endif
