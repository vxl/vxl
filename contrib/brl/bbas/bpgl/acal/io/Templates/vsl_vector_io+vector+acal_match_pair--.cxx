#include <vector>

#include <acal/acal_match_utils.h>
#include <acal/io/acal_io_match_utils.h>

#include <vsl/vsl_vector_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize match nodes
VSL_VECTOR_IO_INSTANTIATE(std::vector<acal_match_pair>);
