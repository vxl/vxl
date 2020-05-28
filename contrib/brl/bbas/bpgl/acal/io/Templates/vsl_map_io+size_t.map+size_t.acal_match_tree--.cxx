#include <functional>
#include <map>

#include <acal/acal_match_tree.h>
#include <acal/io/acal_io_match_tree.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


/* Used to serialize match trees */

VSL_MAP_IO_INSTANTIATE(size_t, acal_match_tree, std::less<size_t>);

typedef std::map<size_t, acal_match_tree> miamt;  // typedef to avoid comma in macro
VSL_MAP_IO_INSTANTIATE(size_t, miamt, std::less<size_t>);
