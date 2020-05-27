#include <vector>

#include <vsl/vsl_vector_io.h>

#include <vsl/vsl_pair_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

// Used to serialize vertex representation
VSL_PAIR_IO_INSTANTIATE(bool, std::vector<size_t>);
