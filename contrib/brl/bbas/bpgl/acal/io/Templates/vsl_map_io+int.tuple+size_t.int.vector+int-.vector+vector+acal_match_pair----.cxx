#include <functional>
#include <tuple>
#include <vector>

#include <acal/acal_match_utils.h>
#include <acal/io/acal_io_match_utils.h>
#include <vsl/vsl_tuple_io.h>
#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_map_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// typedef to avoid comma in macro
typedef std::tuple<size_t, int, std::vector<int>,
                   std::vector<std::vector<acal_match_pair> > > node_representation_type;

// Used to serialize match nodes
/* VSL_VECTOR_IO_INSTANTIATE(int); */
VSL_MAP_IO_INSTANTIATE(int, node_representation_type, std::less<int>);
