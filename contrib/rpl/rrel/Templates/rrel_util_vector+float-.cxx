#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <rrel/rrel_util.hxx>

// Apply explicit instantiation
typedef std::vector<float>::iterator Iter;
RREL_UTIL_INSTANTIATE_RAN_ITER(float, Iter);
