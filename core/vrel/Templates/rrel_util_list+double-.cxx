#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vrel/vrel_util.hxx>

VREL_UTIL_INSTANTIATE_INP_ITER(double, std::list<double>::iterator);
