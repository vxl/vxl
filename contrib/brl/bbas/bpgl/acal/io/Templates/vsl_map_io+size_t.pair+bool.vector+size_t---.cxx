#include <functional>
#include <vector>

#include <vsl/vsl_pair_io.h>
#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// typedef to avoid comma in macro
typedef std::pair<bool, std::vector<size_t> > pbvi;

// Used to serialize vertex representation
VSL_MAP_IO_INSTANTIATE(size_t, pbvi, std::less<size_t>);
