#include <functional>
#include <string>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize image paths
VSL_MAP_IO_INSTANTIATE(size_t, std::string, std::less<size_t>);
