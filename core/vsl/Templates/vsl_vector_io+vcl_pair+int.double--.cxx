#include <utility>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vsl/vsl_pair_io.h"
#include "vsl/vsl_vector_io.hxx"
using pair_id = std::pair<int, double>;
VSL_VECTOR_IO_INSTANTIATE(pair_id);
