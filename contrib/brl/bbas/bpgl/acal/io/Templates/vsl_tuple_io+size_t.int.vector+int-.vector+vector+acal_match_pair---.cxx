#include <vector>

#include <acal/acal_match_utils.h>
#include <acal/io/acal_io_match_utils.h>
#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_tuple_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize match nodes
VSL_TUPLE_IO_INSTANTIATE(size_t, int, std::vector<int>,
                         std::vector<std::vector<acal_match_pair> >);
