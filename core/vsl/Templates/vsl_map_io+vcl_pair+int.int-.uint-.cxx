#include <utility>
#include "vsl/vsl_map_io.hxx"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
using pii = std::pair<int, int>;
using comp = std::less<std::pair<int, int>>;
VSL_MAP_IO_INSTANTIATE(pii, unsigned, comp);
