#include <functional>
#include <tuple>
#include <vector>

#include <acal/acal_match_utils.h>
#include <acal/io/acal_io_match_utils.h>
#include "vsl_tuple_io.h"
#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


typedef std::tuple<size_t, size_t, std::vector<acal_match_pair> > edge_representation_type;

// Used to serialize edges
VSL_MAP_IO_INSTANTIATE(size_t, edge_representation_type, std::less<size_t>);
