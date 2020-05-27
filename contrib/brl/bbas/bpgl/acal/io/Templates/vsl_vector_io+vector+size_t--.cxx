#include <vector>

#include <vsl/vsl_vector_io.hxx>

#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif


// Used to serialize connected components
VSL_VECTOR_IO_INSTANTIATE(std::vector<size_t>);
