#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include <vrel/vrel_util.hxx>

// Apply explicit instantiation
using Iter = std::vector<double>::iterator;
VREL_UTIL_INSTANTIATE_RAN_ITER(double, Iter);
