#include <iostream>
#include <list>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <rrel/rrel_util.hxx>

RREL_UTIL_INSTANTIATE_INP_ITER( double, std::list<double>::iterator );
